#include <imgui.h>
#include "Camera.h"

void Camera::SetLookAt(const DirectX::XMFLOAT3& _eye, const DirectX::XMFLOAT3& _focus, const DirectX::XMFLOAT3& _up)
{
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&_eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&_focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&_up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	this->eye = _eye;
	this->focus = _focus;
}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);

}

void Camera::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Eye", &eye.x);
			ImGui::InputFloat3("Focus", &focus.x);
			ImGui::InputFloat3("Up", &up.x);
			ImGui::InputFloat3("Front", &front.x);
			ImGui::InputFloat3("Right", &right.x);
		}
	}
	ImGui::End();
}

