#pragma once
#include "Component/Component.h"
#include "System/Common/Misc.h"
#include <DirectXMath.h>

class Camera :public Component
{
public:
	Camera() :view(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), projection(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) {}
	~Camera() {}

	void SetLookAt(const DirectX::XMFLOAT3& _eye, const DirectX::XMFLOAT3& _focus, const DirectX::XMFLOAT3& _up);

	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	const DirectX::XMFLOAT4X4& GetView() const { return view; }
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	void DrawDebugGUI();

private:
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	DirectX::XMFLOAT3 eye = { 0,0,0 };
	DirectX::XMFLOAT3 focus = { 0,0,0 };

	DirectX::XMFLOAT3 up = { 0,0,0 };
	DirectX::XMFLOAT3 front = { 0,0,0 };
	DirectX::XMFLOAT3 right = { 0,0,0 };

};

