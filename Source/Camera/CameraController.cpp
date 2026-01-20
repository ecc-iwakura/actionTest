#include "Component/Object.h"
#include "CameraController.h"
#include "Camera.h"
#include "System/Input/Input.h"
#include "System/Common/HighResolutionTimer.h"

CameraController::CameraController() :camera(nullptr), target(0, 0, 0), move(0, 0, 0), moveAngle(0, 0, 0), timer(0),cameraMode(Mode::Fix),rollSpeed(0)
{
}

void CameraController::Start()
{
	camera = parent->GetComponent<Camera>();
	rollSpeed = DirectX::XMConvertToRadians(90);
	cameraMode = Mode::Game;

}

void CameraController::Update()
{
	switch (cameraMode)
	{
	case Mode::Game:
		GameMove();
		break;
	case Mode::Fix:
		// ‰½‚à‚µ‚È‚¢
		break;
	case Mode::Pan:
		PanMove();
		break;
	case Mode::Track:
		TrackMove();
		break;
	default:
		ModeReset();
		break;
	}

	//if (timer > 0.0f)
	//{
	//	timer -= elapsedTime;
	//	if (timer < 0.0f) ModeReset();

	//}

	camera->SetLookAt(transform->position, target, DirectX::XMFLOAT3(0, 1, 0));

}

void CameraController::SetTarget(const DirectX::XMFLOAT3& _target)
{
	if (cameraMode != Mode::Game)return;
	this->target = _target;
}

void CameraController::FixCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _target)
{
	cameraMode = Mode::Fix;
	transform->position = _eye;
	this->target = _target;
}

void CameraController::FixCamera(float ex, float ey, float ez, float fx, float fy, float fz)
{
	DirectX::XMFLOAT3 e(ex, ey, ez);
	DirectX::XMFLOAT3 t(fx, fy, fz);
	FixCamera(e, t);
}

void CameraController::PanCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _moveAngle, float _timer)
{
	cameraMode = Mode::Pan;
	transform->position = _eye;
	this->moveAngle = _moveAngle;
	timer = _timer;

	DirectX::XMFLOAT3 xv = { 1.0f,0.0f,0.0f };
	//DirectX::XMFLOAT3 yv = { 0.0f,1.0f,0.0f };
	DirectX::XMFLOAT3 zv = { 0.0f,0.0f,1.0f };
	DirectX::XMFLOAT3 cv = { target.x - _eye.x,target.y - _eye.y,target.z - _eye.z };

	float len = sqrtf(cv.x * cv.x + cv.y * cv.y + cv.z * cv.z);
	cv.x /= len;
	cv.y /= len;
	cv.z /= len;

	float dotX = zv.z * cv.z;
	if (dotX > 1.0f)dotX = 1.0f;
	else if (dotX < -1.0f)dotX = -1.0f;
	transform->angle.x = acosf(dotX);
	float crossX = zv.z * cv.y;
	if (crossX < 0.0f)transform->angle.x = -transform->angle.x;

	float dotY = xv.x * cv.x;
	if (dotY > 1.0f)dotY = 1.0f;
	else if (dotY < -1.0f)dotY = -1.0f;
	transform->angle.y = acosf(dotY);
	float crossY = xv.x * cv.z;
	if (crossY < 0.0f)transform->angle.y = -transform->angle.y;
}

void CameraController::PanCamera(float ex, float ey, float ez, float fx, float fy, float fz, float _timer)
{
	DirectX::XMFLOAT3 e(ex, ey, ez);
	DirectX::XMFLOAT3 t(fx, fy, fz);
	PanCamera(e, t, _timer);
}

void CameraController::TrackCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _move, float _range, float _timer)
{
	cameraMode = Mode::Track;
	transform->position = _eye;
	this->move = _move;
	this->range = _range;
	timer = _timer;
}

void CameraController::TrackCamera(float ex, float ey, float ez, float mx, float my, float mz, float _range, float _timer)
{
	DirectX::XMFLOAT3 e(ex, ey, ez);
	DirectX::XMFLOAT3 m(mx, my, mz);
	TrackCamera(e, m, _range, _timer);
}

void CameraController::LockCamera(DirectX::XMFLOAT3& _target)
{
	this->target = _target;
}

void CameraController::GameMove()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();
	float speed = rollSpeed * Time::DeltaTime();

	transform->angle.y += ax * speed;
	transform->angle.x += ay * speed;

	if (transform->angle.x < -DirectX::XM_PIDIV2) transform->angle.x = -DirectX::XM_PIDIV2;
	if (transform->angle.x > DirectX::XM_PIDIV2) transform->angle.x = DirectX::XM_PIDIV2;
	if (transform->angle.y < -DirectX::XM_PI) transform->angle.y += DirectX::XM_2PI;
	if (transform->angle.y > DirectX::XM_PI) transform->angle.y -= DirectX::XM_2PI;

	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(transform->angle.x, transform->angle.y, transform->angle.z);

	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	transform->position.x = -front.x * range + target.x;
	transform->position.y = -front.y * range + target.y;
	transform->position.z = -front.z * range + target.z;
}

void CameraController::PanMove()
{
	transform->angle.x += moveAngle.x * Time::DeltaTime();
	transform->angle.y += moveAngle.y * Time::DeltaTime();
	//angle.z += moveAngle.z * elapsedTime;

	if (transform->angle.x < -DirectX::XM_PIDIV2) transform->angle.x = -DirectX::XM_PIDIV2;
	if (transform->angle.x > DirectX::XM_PIDIV2) transform->angle.x = DirectX::XM_PIDIV2;
	if (transform->angle.y < -DirectX::XM_PI) transform->angle.y += DirectX::XM_2PI;
	if (transform->angle.y > DirectX::XM_PI) transform->angle.y -= DirectX::XM_2PI;

	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(transform->angle.x, transform->angle.y, transform->angle.z);

	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	target.x = front.x * range + transform->position.x;
	target.y = front.y * range + transform->position.y;
	target.z = front.z * range + transform->position.z;
}

void CameraController::TrackMove()
{
	DirectX::XMFLOAT3 moveVec(move.x * Time::DeltaTime(), move.y * Time::DeltaTime(), move.z * Time::DeltaTime());
	transform->position.x += moveVec.x;
	transform->position.y += moveVec.y;
	transform->position.z += moveVec.z;
	target.x += moveVec.x;
	target.y += moveVec.y;
	target.z += moveVec.z;
}

void CameraController::CameraAngleReset()
{
	transform->angle.x = 0.0f;
	target.y = transform->position.y;
}

void CameraController::ModeReset()
{
	cameraMode = Mode::Game;
	move = { 0,0,0 };
	moveAngle = { 0,0,0 };
	timer = 0.0f;
}

