#pragma once

#include "Component/Component.h"
#include "Camera/Camera.h"
#include <DirectXMath.h>
// Effekseer関連のWarningを消す
#pragma warning( push )
#pragma warning( disable : 6385 )
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#pragma warning( pop )

class EffectManager :public Component
{
public:
	EffectManager() :camera(nullptr) {}
	~EffectManager() {}

	// 初期化
	void Start();
	// 終了化
	void End();
	// 更新処理
	void Update();
	// 描画処理
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	// Effeckseerマネージャーの取得
	Effekseer::ManagerRef GetManagerRef() const { return managerRef; }
	void SetCamera(Camera* cam);
private:
	Effekseer::ManagerRef managerRef;
	EffekseerRenderer::RendererRef effekseerRenderer;

	Camera* camera;
};