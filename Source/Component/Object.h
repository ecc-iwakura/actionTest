#pragma once
#include "Component.h"
#include "System/Common/Logger.h"
#include "System/Common/Misc.h"
#include "System/Graphics/Shader.h"
#include "Transform.h"
#include <d3d11.h>
#include <list>
#include <typeinfo>

class Object
{
private:
	// コンポーネントリスト
	std::list<Component*> componentList;

	// 生存フラグ
	bool alive;

public:
	// 位置、回転、スケールを管理するクラス
	// 行列は全てのコンポーネントのアップデートが終わったあとに更新される
	// Object クラスで実体の生成及び削除を行うので、外部で new や delete をしないこと
	Transform* transform;

	Object() :alive(true) { transform = new Transform(); }
	~Object() {
		for (auto& com : componentList) SafeDelete(com);
		componentList.clear();
		SafeDelete(transform);
	}

	void Update()
	{
		// 生存フラグが立っていない場合は何もしない
		if (!alive)return;

		auto& buff = componentList;
		for (auto& com : buff)
		{
			com->Update();
			// 生存フラグが立っていない場合は処理を終了
			if (!alive)return;
		}
		// Transformの行列を最後に更新
		transform->Update();
	}

	void Render(ID3D11DeviceContext* dc, Shader* shader) const
	{
		for (auto& com : componentList) com->Render(dc, shader);
	}

	void Render2D(ID3D11DeviceContext* dc) const
	{
		for (auto& com : componentList) com->Render2D(dc);
	}

	void RenderDebugGUI()
	{
		for (auto& com : componentList) com->RenderDebugGUI();
	}

	void Destroy()
	{
		// 生存フラグを折る
		alive = false;
	}

	bool IsAlive() const
	{
		return alive;
	}

	//オブジェクトリスト内の指定したコンポーネントを取得
	template<class T>
	T* GetComponent() const
	{
		for (auto& com : componentList) {
			if (typeid( *com ) == typeid( T ))
				return static_cast<T*>( com );
		}
		return nullptr;
	}

	//オブジェクトリストにコンポーネントを追加(引数なし)
	template<class T>
	T* AddComponent()
	{
		// 生存フラグが立っていない場合は何もしない
		if (!alive)return nullptr;

		// Componentクラスを継承していないクラスを追加しようとした場合
		// 警告文を表示して nullptr を返す
		if (std::is_base_of<Component, T>::value == false)
		{
			Logger::Print("Componentクラスを継承していないクラスを追加しようとしました。　データ型：%s", typeid( T ).name());
			return nullptr;
		}

		// コンポーネントの作成
		// 引数の内容の有無で処理を切り替える
		Component* buff = new T();
		// 親子関係を設定
		buff->parent = this;
		buff->transform = this->transform;
		// コンポーネントリストに追加
		componentList.emplace_back(buff);
		// コンポーネントの初期化処理実行
		buff->Start();
		// 追加したコンポーネントを返す
		return static_cast<T*>( buff );
	}
	//オブジェクトリストにコンポーネントを追加(引数あり)
	template<class T>
	T* AddComponent(const char* filename)
	{
		// 生存フラグが立っていない場合は何もしない
		if (!alive)return nullptr;

		// Componentクラスを継承していないクラスを追加しようとした場合
		// 警告文を表示して nullptr を返す
		if (std::is_base_of<Component, T>::value == false)
		{
			Logger::Print("Componentクラスを継承していないクラスを追加しようとしました。　データ型：%s", typeid( T ).name());
			return nullptr;
		}

		// コンポーネントの作成
		// 引数の内容の有無で処理を切り替える
		Component* buff = new T(filename);
		// 親子関係を設定
		buff->parent = this;
		buff->transform = this->transform;
		// コンポーネントリストに追加
		componentList.emplace_back(buff);
		// コンポーネントの初期化処理実行
		buff->Start();
		// 追加したコンポーネントを返す
		return static_cast<T*>( buff );
	}
};

#include "System/Common/HashOrder.h"
#include <string>
#include <unordered_map>

class ObjectList
{
private:
	std::unordered_map<std::string, Object*> objectList;

public:
	ObjectList()
	{
		objectList.clear();
	}
	~ObjectList()
	{
		objectList.clear();
	}
	// 要素をリストに追加
	// 要素名は自動生成
	bool Push(Object* obj)
	{
		return Push(HashOrder::Create(), obj);
	}
	// 要素名を指定してリスト要素を追加
	// 成功した場合はtrue、失敗した場合はfalseを返す
	bool Push(std::string objName, Object* obj)
	{
		// 要素の追加を試行。戻り値はretに保存
		std::pair < std::unordered_map<std::string, Object*>::iterator, bool> ret = objectList.try_emplace(objName, obj);
		// retのsecondがfalseの場合、追加に失敗しているのでエラーメッセージを表示
		if (ret.second == false) Logger::Print("要素の追加に失敗しました");
		// 追加の結果(true/false)を返す
		return ret.second;
	}

	// 指定要素名のオブジェクトを取得する関数
	// 指定のオブジェクトが存在しない場合はnullptrを返す
	Object* Find(std::string objName)
	{
		// find関数でObjectリストから指定の要素のイテレーターを取得する
		auto it = objectList.find(objName);
		// Objectリストのend()が返ってきたら要素が無いのでエラーメッセージとnullptrを返す
		if (it == objectList.end())
		{
			Logger::Print("指定したオブジェクトは存在しませんでした");
			return nullptr;
		}
		// nullptr以外が返ってきた場合は要素が存在しているのでObject(second)を返す
		return it->second;
	}

	// 指定要素名のオブジェクトを削除する関数
	// 指定のオブジェクトが存在しない場合は何もしない
	void Erase(std::string objName)
	{
		// find関数でObjectリストから指定の要素のイテレーターを取得する
		auto it = objectList.find(objName);
		// Objectリストのend()が返ってきたら要素が無いのでエラーメッセージを表示して何もしない
		if (it == objectList.end())
		{
			Logger::Print("指定したオブジェクトは存在しないため削除できませんでした");
			return;
		}
		// 取得した要素を削除
		objectList.erase(it);
	}

	// リストの要素を全削除する関数
	void Clear()
	{
		for (auto& obj : objectList)	SafeDelete(obj.second);
		objectList.clear();
	}

	// 更新処理
	void Update()
	{
		// 全オブジェクト更新
		// Update中にObjectListがいじられてイテレーションがバグるのを回避
		auto& buff = objectList;
		for (auto& obj : buff)
		{
			// 更新処理
			obj.second->Update();
			// 生存確認
			// 生存フラグが立っていない場合はオブジェクトリストから削除する
			if (!obj.second->IsAlive())buff.erase(obj.first);
		}
	}
	// 全オブジェクト描画(3D)
	void Render(ID3D11DeviceContext* dc, Shader* shader) const
	{
		for (auto& obj : objectList)	obj.second->Render(dc, shader);
	}

	// 全オブジェクト描画(2D)
	void Render2D(ID3D11DeviceContext* dc) const
	{
		for (auto& obj : objectList)	obj.second->Render2D(dc);
	}

	void RenderDebugGUI()
	{
		for (auto& obj : objectList)	obj.second->RenderDebugGUI();
	}
};