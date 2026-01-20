#pragma once

#include "Camera/Camera.h"
#include "Character.h"
#include "System/Graphics/Model.h"
#include "System/Graphics/Shader.h"

class Player : public Character
{
private:
	Model* model = nullptr;

	Camera* camera;

	void GetMoveVec();
	void Move(float elapsedTime, float vx, float vz, float speed);
	void InputMove(float elapsedTime);
	void Turn(float elapsedTime, float vx, float vz, float speed);

public:
	Player();
	~Player() override;

	void Start();

	void Update();
	void Render(ID3D11DeviceContext* dc, Shader* shader)const;
	void DrawDebugGUI();

	void SetCamera(Camera* cam);

	DirectX::XMFLOAT3 moveVec = { 0,0,0 };
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

};

