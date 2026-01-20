#pragma once
#include "System/Graphics/Shader.h"
#include <d3d11.h>

class Object;
class Transform;

class Component
{
protected:
public:
	Component() :parent(nullptr), transform(nullptr) {}
	Component(const char* filename) :Component() {}
	virtual ~Component() {}

	// 親オブジェクトのポインタ
	Object* parent;
	// parent が持つ transform のアドレス
	Transform* transform;

	// 初期化関数
	// コンポーネントを追加したときに呼び出される
	virtual void Start() {}
	// 更新処理
	virtual void Update() {}
	// 終了処理
	// コンポーネントを削除する前に呼び出される
	virtual void End() {}
	// 3D描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) const {}
	// 2D描画処理
	virtual void Render2D(ID3D11DeviceContext* dc) const {}
	// デバッグ表示
	virtual void RenderDebugGUI() {}
};

