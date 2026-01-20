#pragma once
#include "Component.h"
#include "Transform.h"
#include "System/Common/HighResolutionTimer.h"
#include "System/Common/XMFloatHelper.h"

class Movement :public Component
{
public:
	DirectX::XMFLOAT3A velocity;
	DirectX::XMFLOAT3A acceleration;
	bool useGravity;

	Movement() :velocity(0, 0, 0), acceleration(0, 0, 0), useGravity(false) {}

	void Update()    
	{
		// ‰Á‘¬“x‚ðˆÚ“®’l‚É‰ÁŽZ
		velocity += acceleration;

		// d—ÍŒvŽZ
		if (useGravity)velocity.y += 0.98f;

		// ˆÚ“®ˆ—
		transform->position += velocity * Time::DeltaTime();
	}
};