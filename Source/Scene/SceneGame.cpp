#include "Camera/Camera.h"
#include "Component/Object.h"
#include "SceneGame.h"
#include "System/Audio/AudioListener.h"
#include "System/Audio/AudioPlayer.h"
#include "System/Audio/AudioManager.h"
#include "System/Graphics/Graphics.h"
#include "System/Input/Input.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"


SceneGame::~SceneGame()
{
	Finalize();
}

// 初期化
void SceneGame::Initialize()
{
	// カメラ初期化
	{
		Object* obj = new Object();
		camera = obj->AddComponent<Camera>();
		cameraController = obj->AddComponent<CameraController>();
		audioListener = obj->AddComponent<AudioListener>();
		objectList.Push("camera", obj);
	}
	// ステージ初期化
	{
		Object* obj = new Object();
		stage = obj->AddComponent<Stage>();
		objectList.Push("stage", obj);
	}
	// プレイヤー初期化
	{
		Object* obj = new Object();
		player = obj->AddComponent<Player>();
		player->SetCamera(camera);
		//obj->AddComponent<AudioPlayer>()->CreateSourceVoice(AudioManager::Instance()->GetSESubmixVoice());
		objectList.Push("player", obj);
	}
	// エフェクトマネージャー初期化
	{
		Object* obj = new Object();
		effectManager = obj->AddComponent<EffectManager>();
		effectManager->SetCamera(camera);
		objectList.Push("effectManager", obj);
	}
	{
		Object* obj = new Object();
		effect = obj->AddComponent<Effect>("./Data/Effect/explode1.efk");
		audioPlayer = obj->AddComponent<AudioPlayer>();
		audioPlayer->CreateSourceVoice(AudioManager::Instance()->GetSESubmixVoice());
		effect->Play({ 0,0,0 });
		objectList.Push("effect", obj);
	}
	AudioManager::Instance()->LoadSE("./Data/Audio/explode_mono.wav", audio::SE::SE1);

	Graphics& graphics = Graphics::Instance();
	camera->SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera->SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
}

// 終了化
void SceneGame::Finalize()
{
	objectList.Clear();
}

// 更新処理
void SceneGame::Update()
{
	// 入力情報
	Input input = Input::Instance();

	// 全オブジェクト更新
	objectList.Update();

	if (player)
	{
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
	}
	if (input.GetGamePad().GetButtonDown() & input.GetGamePad().BTN_A)
	{
		effect->Play({ 0,0,0 });
		audioPlayer->Play(audio::SE::SE1);
	}
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc = {};
	rc.lightDirection = { 0.5f, -1.0f, 0.5f, 0.0f };	// ライト方向

	// カメラパラメータ設定
	rc.view = camera->GetView();
	rc.projection = camera->GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		// 全オブジェクトを描画
		objectList.Render(dc, shader);

		shader->End(dc);
	}

	// エフェクト描画
	effectManager->Render(rc.view, rc.projection);

	// 3Dデバッグ描画
	{
		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		// 全オブジェクトを描画
		objectList.Render2D(dc);
	}

	// 2DデバッグGUI描画
	{
		if (player)player->DrawDebugGUI();
		if (camera)camera->DrawDebugGUI();
		objectList.RenderDebugGUI();
	}
}
