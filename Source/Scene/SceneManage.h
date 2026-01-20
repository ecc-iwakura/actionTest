#pragma once
#include "Scene.h"
#include "SceneGame.h"

enum class SceneTag
{
	Game,
	SceneMax
};

class SceneManager
{
private:
	static SceneManager* instance;

	// 現在進行しているシーンのポインタ
	Scene* pScene;
	// 次に移行するシーンのポインタ
	Scene* pNextScene;

	// 各シーンの実体
	SceneGame sceneGame;

	// 全シーンのリスト
	Scene* sceneList[static_cast<int>( SceneTag::SceneMax )] = {
		&sceneGame
	};

public:
	SceneManager();
	~SceneManager() {}

	// 遷移したいシーンの予約を行うための関数
	// 次フレームのSceneManagerのUpdate関数の最初にシーン切り替え処理が行われる
	void ChangeScene(SceneTag tag);

	void Update();
	void Render();

	// 現在進行しているシーンを取得する関数
	Scene* GetScene() const;

	// 引数で指定したSceneを返す
	// SceneMaxが指定された時はnullptrを返す
	Scene* GetScene(SceneTag tag) const;

	static inline SceneManager* Instance()
	{
		return instance;
	}
};
