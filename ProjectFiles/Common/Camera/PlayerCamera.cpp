#include "PlayerCamera.h"
#include "Input.h"
#include "CommandList.h"
#include "Game.h"
#include "MathHelp.h"
#include "Object/Gate/Gate.h"
#include "Collider/ColliderCapsule.h"

namespace
{
	// 上下回転限界値
	constexpr float LIMIT_VERTEX_ANGLE = 89.0f;
	// 180度ターンに必要なもの
	constexpr int TURN_FRAME = 10;
	const Quaternion TURN_QUATERNION = Quaternion::AngleAxis(180 / TURN_FRAME, Vec3::Up());
}

PlayerCamera::PlayerCamera() :
	m_frame(0),
	m_isTurn(false)
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Update(const Vec3& playerPos)
{
	constexpr float VERTEX_ANGLE_SPEED = -1.0f;
	constexpr float HORIZONTAL_ANGLE_SPEED = 1.0f;
	// カメラの回転
	auto& input = Input::GetInstance();
	const auto& trigger = input.GetTriggerData().RStick;
	m_info.targetPos = playerPos;
	// 左右回転
	const auto& rot = Quaternion::AngleAxis(trigger.x * HORIZONTAL_ANGLE_SPEED, Vec3::Up());
	m_info.front = rot * m_info.front;
	m_info.right = rot * m_info.right;
	// 上下回転
	m_info.vertexAngle = std::min<float>(std::max<float>(m_info.vertexAngle + trigger.y * VERTEX_ANGLE_SPEED, -LIMIT_VERTEX_ANGLE), LIMIT_VERTEX_ANGLE);
	
	// 180度回転
	if (m_isTurn) 
	{
		m_frame--;
		if (m_frame < 0)
		{
			m_isTurn = false;
		}
		else
		{
			m_info.front = TURN_QUATERNION * m_info.front;
			m_info.right = TURN_QUATERNION * m_info.right;
		}
	}
	else if (input.IsTriggerd(Command::BTN_TURN_CAMERA))
	{
		m_frame = TURN_FRAME;
		m_isTurn = true;
	}

	// FPS、TPS切り替え
	if (input.IsTriggerd(Command::BTN_CHANGE_VIEW))
	{
		m_info.isTps = !m_info.isTps;
	}
}

void PlayerCamera::OnWarp(const Vec3& preVelDir, const Vec3& newVelDir, const Vec3& pos)
{
	const auto& rotBase = Quaternion::GetQuaternion(preVelDir, newVelDir);
	const auto& euler = Quaternion::GetEuler(rotBase);
	const auto& rot = Quaternion::AngleAxis(euler.t1.y, Vec3::Up());
	m_info.front = rot * m_info.front;
	m_info.right = rot * m_info.right;
	m_info.targetPos = pos;

	auto newPos = m_info.targetPos - m_info.front * GetDistance();
	m_info.cameraPos = newPos;
}
