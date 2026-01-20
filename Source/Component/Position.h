#pragma once
//#include <DirectXMath.h>
#include "Component.h"

class Position2D : public Component
{
public:
	float x, y;
};

class Position : public Component
{
public:
	float x, y, z;
};

//class Transform : public Component
//{
//public:
//	DirectX::XMFLOAT3 position;
//	DirectX::XMFLOAT3 rotation;
//	DirectX::XMFLOAT3 scale;
//};

