#include "SceneManage.h"

SceneManager* SceneManager::instance = nullptr;

SceneManager::SceneManager()
{
	pScene = nullptr;
	pNextScene = nullptr;
	instance = this;
}

void SceneManager::ChangeScene(SceneTag tag)
{
	// 移行するシーンを設定する
	pNextScene = sceneList[static_cast<int>( tag )];
}

void SceneManager::Update()
{
	// シーンの切り替え処理
	if (pNextScene)
	{
		// 進行中のシーンが設定されていて、
		// 違うシーンに移行する場合は開放処理を呼び出す
		if (pScene && pScene != pNextScene)pScene->Finalize();
		// シーンを切り替えて初期化する
		pScene = pNextScene;
		pScene->Initialize();
		// 移行が終了したのでnullptrを設定しておく
		pNextScene = nullptr;
	}

	// シーンの更新処理
	pScene->Update();
}

void SceneManager::Render()
{
	// シーンの描画処理
	pScene->Render();
}

Scene* SceneManager::GetScene() const
{
	return pScene;
}

Scene* SceneManager::GetScene(SceneTag tag) const
{
	return tag == SceneTag::SceneMax ? nullptr : sceneList[static_cast<int>( tag )];
}

