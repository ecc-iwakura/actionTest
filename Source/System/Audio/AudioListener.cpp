#include "AudioListener.h"
#include "AudioManager.h"
#include "Component/Object.h"
#include "System/Common/Logger.h"	
#include <imgui.h>

int AudioListener::instanceCount = 0;

AudioListener::AudioListener()
{
	if (instanceCount != 0) Logger::Print("AudioListenerのインスタンスが1つ以上存在します");
	instanceCount++;
}

AudioListener::~AudioListener()
{
	if (instanceCount > 1) Logger::Print("AudioListenerのインスタンスが1つ以上存在していました");
	instanceCount--;
}

void AudioListener::Start()
{
	// 3Dサウンドリスナーの取得
	listener = AudioManager::Instance()->Get3DAudioListener();

	// 移動コンポーネントを取得
	movement = parent->GetComponent<Movement>();

	// 3Dサウンドリスナーの初期化
	listener->Position = transform->position;
	listener->Velocity = { 0,0,0 };
	listener->OrientFront = { 0,0,0 };
	listener->OrientTop = { 0,1,0 };

	cone.InnerAngle = 0.0f;
	cone.OuterAngle = X3DAUDIO_PI;
	cone.InnerVolume = 1.0f;
	cone.OuterVolume = 0.5f;
	cone.InnerLPF = 1.0f;
	cone.OuterLPF = 1.0f;
	cone.InnerReverb = 0.0f;
	cone.OuterReverb = 0.0f;

	//listener->pCone = &cone;
	cone = X3DAudioDefault_DirectionalCone;
	listener->pCone = &cone;
}

void AudioListener::Update()
{
	// リスナーの位置更新
	listener->Position = transform->position;
	// リスナーの移動量&方向更新
	if (movement)listener->Velocity = movement->velocity;
	// リスナーの向き更新
	// 今回はY軸回転以外考慮しない
	DirectX::XMFLOAT3 angle = transform->angle;
	listener->OrientFront = { sinf(angle.y),0,cosf(angle.y) };
	listener->OrientTop = { 0,1,0 };
}

void AudioListener::RenderDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("AudioListener", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Listener", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Front", &listener->OrientFront.x);
			ImGui::InputFloat3("Position", &listener->Position.x);
			ImGui::InputFloat3("Velocity", &listener->Velocity.x);
		}
	}
	ImGui::End();
}

