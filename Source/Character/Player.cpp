#include "Camera/Camera.h"
#include "Player.h"
#include "System/Common/HighResolutionTimer.h"
#include "System/Common/Misc.h"
#include "System/Input/Input.h"
#include <imgui.h>


void Player::GetMoveVec()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	if (!camera)return;

	const DirectX::XMFLOAT3& cameraRight = camera->GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera->GetFront();

	float cameraRightX = cameraRight.x * cameraRight.x;
	float cameraRightZ = cameraRight.z * cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX + cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		cameraRightX = cameraRight.x / cameraRightLength;
		cameraRightZ = cameraRight.z / cameraRightLength;
	}

	float cameraFrontX = cameraFront.x * cameraFront.x;
	float cameraFrontZ = cameraFront.z * cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX + cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX = cameraFront.x / cameraFrontLength;
		cameraFrontZ = cameraFront.z / cameraFrontLength;
	}

	moveVec.x = cameraRightX * ax + cameraFrontX * ay;
	moveVec.z = cameraRightZ * ax + cameraFrontZ * ay;
	moveVec.y = 0.0f;

}

void Player::Move(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;
}

void Player::InputMove(float elapsedTime)
{
	GetMoveVec();

	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

void Player::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	if (vx == 0.0f && vz == 0.0f)return;

	float len = sqrtf(vx * vx + vz * vz);
	vx /= len;
	vz /= len;

	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	float dot = ( vx * frontX ) + ( vz * frontZ );

	float rot = 1.0f - dot;

	float cross = ( vz * frontX ) - ( vx * frontZ );

	if (cross < 0.0f)
	{
		angle.y += sinf(rot) * speed;
	}
	else
	{
		angle.y -= cosf(rot) * speed;
	}

	if (angle.x < -DirectX::XM_PI) angle.x += DirectX::XM_2PI;
	if (angle.x > DirectX::XM_PI) angle.x -= DirectX::XM_2PI;
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
	if (angle.z < -DirectX::XM_PI) angle.z += DirectX::XM_2PI;
	if (angle.z > DirectX::XM_PI) angle.z -= DirectX::XM_2PI;

}

Player::Player() :camera(nullptr)
{
}

Player::~Player()
{
	SafeDelete(model);
}

void Player::Start()
{
	model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	//camera = SceneManager::Instance()->GetScene(SceneTag::Game)->objectList.at("camera")->GetComponent<Camera>();

	scale.x = scale.y = scale.z = 0.01f;
}

void Player::Update()
{

	InputMove(Time::DeltaTime());

	UpdateTransform();
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)const
{
	shader->Draw(dc, model);
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Position", &position.x);

			DirectX::XMFLOAT3 a(0, 0, 0);
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}

void Player::SetCamera(Camera* cam)
{
	if (cam != nullptr)camera = cam;
}
