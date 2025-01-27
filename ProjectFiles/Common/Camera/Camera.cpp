#include "Camera.h"
#include <DxLib.h>
#include "Game.h"
#include "Easing.h"
#include "Collider/ColliderSphere.h"
#include "Object/Gate/Gate.h"
#include "Object/Gate/GateManager.h"

namespace
{
	constexpr float DEF_NEAR = 0.75f;
	constexpr float DEF_FAR = 90.0f;

	constexpr float DEF_FOV_ANGLE = 60.0f;
	constexpr float DEF_FOV_RADIAN = DEF_FOV_ANGLE * Game::DEG_2_RAD;

	constexpr float CAMERA_DISTANCE = 10.0f;
	const Vec3 DIR_RIGHT_BASE = Vec3(-1, 0, 0);
	const Vec3 DIR_FRONT_BASE = Vec3(0, 0, -1);
	constexpr float START_VERTEX_ANGLE = 0.0f;
	const Quaternion START_VERTEX_ROT = Quaternion::AngleAxis(START_VERTEX_ANGLE, DIR_RIGHT_BASE);
}

Camera::Camera()
{
	m_info.n = DEF_NEAR;
	m_info.f = DEF_FAR;
	m_info.fov = DEF_FOV_RADIAN;
	m_info.front = DIR_FRONT_BASE;
	m_info.right = DIR_RIGHT_BASE;
	m_info.vertexAngle = START_VERTEX_ANGLE;
	m_info.vertexRot = START_VERTEX_ROT;
	m_info.look = m_info.vertexRot * m_info.front;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// 位置を更新
	m_info.vertexRot = Quaternion::AngleAxis(m_info.vertexAngle, m_info.right);
	m_info.look = m_info.vertexRot * m_info.front;
	auto newPos = m_info.targetPos - m_info.look * CAMERA_DISTANCE;

	auto& nowPos = m_info.cameraPos;
	nowPos = Easing::Lerp(nowPos, newPos, 0.1f);
}

void Camera::SetNearFar(float n, float f)
{
	if (n < 0) n = DEF_NEAR;
	m_info.n = n;
	if (f < 0) f = DEF_FAR;
	m_info.f = f;
}

void Camera::SetFov(float angle)
{
	if (angle < 0) angle = DEF_FOV_ANGLE;
	m_info.fov = angle * Game::DEG_2_RAD;
}

void Camera::SetFront(const Vec3& front, float vertexAngle)
{
	const auto& nFront = front.GetNormalized();
	const auto& q = Quaternion::GetQuaternion(m_info.front, nFront);
	m_info.front = nFront;
	m_info.right = q * m_info.right;
	m_info.vertexAngle = vertexAngle;
	m_info.vertexRot = Quaternion::AngleAxis(m_info.vertexAngle, m_info.right);
	m_info.look = m_info.vertexRot * m_info.front;
}

/// <summary>
/// 初期位置
/// </summary>
/// <param name="targetPos">ターゲットの座標</param>
void Camera::SetTargetPos(const Vec3& targetPos)
{
	m_info.targetPos = targetPos;
	auto cameraPos = targetPos - m_info.look * CAMERA_DISTANCE;
	m_info.cameraPos = cameraPos;
}

void Camera::SetTps(bool isTps)
{
	m_info.isTps = isTps;
}

void Camera::AppInfo()
{
	SetCameraNearFar(m_info.n, m_info.f);
	SetupCamera_Perspective(m_info.fov);
	if (m_info.isTps)
	{
		SetCameraPositionAndTarget_UpVecY(m_info.cameraPos.VGet(), m_info.targetPos.VGet());
	}
	else
	{
		SetCameraPositionAndTarget_UpVecY(m_info.targetPos.VGet(), (m_info.targetPos + m_info.look * CAMERA_DISTANCE).VGet());
	}
}

const Vec3& Camera::GetPos() const
{
	if (m_info.isTps)
	{
		return m_info.cameraPos;
	}
	else
	{
		return m_info.targetPos;
	}
}

float Camera::GetDistance() const
{
	return CAMERA_DISTANCE;
}
