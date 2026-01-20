#include "Effect.h"
#include "Component/Object.h"
#include "Scene/SceneManage.h"


Effect::Effect(const char* filename)
{
	// Effekseerのリソースを読み込む
	// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	// Effekseer::Managerを取得
	Object* obj = SceneManager::Instance()->GetScene()->objectList.Find("effectManager");
	effekseerManager = obj->GetComponent<EffectManager>()->GetManagerRef();

	// Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, utf16Filename);
}

Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& pos, float scale)
{
	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, pos.x, pos.y, pos.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
	effekseerManager->StopEffect(handle);
}

void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& pos)
{
	effekseerManager->SetLocation(handle, pos.x, pos.y, pos.z);
}

void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}
