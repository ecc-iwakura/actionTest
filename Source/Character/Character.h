#pragma once
#include <DirectXMath.h>
#include "Component/Component.h"

class Character:public Component
{
public:
	Character(){}
	virtual ~Character(){}

	void UpdateTransform();

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	void SetPotision(const DirectX::XMFLOAT3& _position) { this->position = _position; }
	
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	void SetAngle(const DirectX::XMFLOAT3& _angle) { this->angle = _angle; }
	
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	void SetScale(const DirectX::XMFLOAT3& _scale) { this->scale = _scale; }


protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 0,0,0 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};

