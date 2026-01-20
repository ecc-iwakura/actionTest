#pragma once
#include "Camera/CameraController.h"
#include "Character/Player.h"
#include "Effect/Effect.h"
#include "Effect/EffectManager.h"
#include "Scene.h"
#include "Stage/Stage.h"
#include "System/Audio/AudioListener.h"
#include "System/Audio/AudioPlayer.h"

// ゲームシーン
class SceneGame : public Scene
{
private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	Camera* camera = nullptr;
	CameraController* cameraController = nullptr;
	EffectManager* effectManager = nullptr;
	AudioListener* audioListener = nullptr;
	AudioPlayer* audioPlayer = nullptr;
	Effect* effect = nullptr;

public:
	SceneGame() {}
	~SceneGame();

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update();

	// 描画処理
	void Render();
};
