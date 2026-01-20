#pragma once
#include "Component/Component.h"
#include <DirectXMath.h>
// Effekseer関連のWarningを消す
#pragma warning( push )
#pragma warning( disable : 6385 )
#include <Effekseer.h>
#pragma warning( pop )

class Effect:public Component
{
public:
	Effect(){}
	Effect(const char* filename);
	~Effect(){}

	// 再生
	Effekseer::Handle Play(const DirectX::XMFLOAT3& pos, float scale = 1.0f);

	// 停止
	void Stop(Effekseer::Handle handle);

	// 座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& pos);

	// スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

private:
	// 再生するエフェクト
	Effekseer::EffectRef effekseerEffect = nullptr;

	// Effekseerマネージャーのポインタ
	Effekseer::ManagerRef effekseerManager = nullptr;
};