#include "EffectManager.h"
#include "System/Common/HighResolutionTimer.h"
#include "System/Graphics/Graphics.h"

void EffectManager::Start()
{
	Graphics& graphics = Graphics::Instance();

	// Effekseer レンダラ生成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

	// Effekseerマネージャー生成
	managerRef = Effekseer::Manager::Create(2048);

	// Effekseerレンダラの各種設定(特別なカスタマイズをしない場合は定型的に以下の設定でOK)
	managerRef->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	managerRef->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	managerRef->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	managerRef->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	managerRef->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// Effekseer内でのローダーの設定(特別なカスタマイズをしない場合は以下の設定でOK)
	managerRef->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	managerRef->SetModelLoader(effekseerRenderer->CreateModelLoader());
	managerRef->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	// Effekseerを左手座標系で計算する
	managerRef->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::End()
{
	// EffekseerManagerなどはスマートポインタによって破棄されるので何もしない
}

void EffectManager::Update()
{
	// エフェクト更新処理(引数にはフレームの経過時間を渡す)
	managerRef->Update();
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// カメラ情報がセットされていなければ何もしない
	if (camera == nullptr)return;

	// ビュー&プロジェクション行列をEffekseerレンダラに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>( &camera->GetView() ));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>( &camera->GetProjection() ));

	// Effekseer描画開始
	effekseerRenderer->BeginRendering();

	// Effekseer描画実行
	// マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個生成し、
	// Draw()カンスを実行する順序で制御できそう
	managerRef->Draw();

	// Effekseer描画終了
	effekseerRenderer->EndRendering();
}

void EffectManager::SetCamera(Camera* cam)
{
	// 引数がnullptrでなければセットする
	if (cam)camera = cam;
}
