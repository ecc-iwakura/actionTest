#pragma once

#include "Component/Component.h"

class Transform :public Component
{
public:
	Transform() { Start(); }
	virtual ~Transform() {}
	void Start() override
	{
		position = { 0,0,0 };
		angle = { 0,0,0 };
		scale = { 1,1,1 };
		transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		};
	}

	void Update() override
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMStoreFloat4x4(&transform, W);
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 transform;
};
