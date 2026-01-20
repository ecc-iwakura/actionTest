#pragma once
#include "Camera/Camera.h"
#include "Component/Component.h"
#include <DirectXMath.h>

class CameraController :public Component
{
public:
	CameraController();
	~CameraController() {}
	void Start()override;
	void Update();

	void SetTarget(const DirectX::XMFLOAT3& _target);

	// 固定カメラ
	void FixCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _target);
	void FixCamera(float ex, float ey, float ez, float fx, float fy, float fz);
	// パンカメラ(カメラの向きだけ変える)
	void PanCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _moveAngle, float _timer);
	void PanCamera(float ex, float ey, float ez, float fx, float fy, float fz, float _timer);
	// 平行移動カメラ
	void TrackCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _move, float _range, float _timer);
	void TrackCamera(float ex, float ey, float ez, float mx, float my, float mz, float _range, float _timer);

	// 注視カメラ
	void LockCamera(DirectX::XMFLOAT3& _target);


	// カメラの向きを水平にリセット
	void CameraAngleReset();

	void ModeReset();

private:
	// ゲームプレイ用カメラ
	void GameMove();
	void PanMove();
	void TrackMove();

	enum class Mode
	{
		Game,
		Fix,
		Pan,
		Track,
	};

private:
	// カメラコンポーネント
	Camera* camera;

	DirectX::XMFLOAT3 target = { 0,0,0 };
	DirectX::XMFLOAT3 move = { 0,0,0 };
	DirectX::XMFLOAT3 moveAngle = { 0,0,0 };

	float timer;

	Mode cameraMode;

	float rollSpeed;
	float range = 10.0f;

};

