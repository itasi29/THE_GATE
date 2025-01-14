#include "GateCamera.h"
#include "Game.h"
#include "Object/Gate/Gate.h"

GateCamera::GateCamera()
{
	m_info.isTps = false;
}

GateCamera::~GateCamera()
{
}

void GateCamera::Update(const CameraInfo& playerCamera, const Vec3& pairBase)
{
	m_info.vertexAngle = playerCamera.vertexAngle * -1;


	m_info.front = m_baseViewDir;

	const auto& baseXZ = Vec3(-pairBase.x, 0, -pairBase.z);
	if (baseXZ.SqLength() > 0)
	{
		const auto& rot = Quaternion::GetQuaternion(baseXZ.GetNormalized(), playerCamera.front);
		const auto& euler = Quaternion::GetEuler(rot);
		m_info.front = rot * m_baseViewDir;
	}
	else
	{
		m_info.front = m_baseViewDir;
	}

	m_info.right = Vec3::Cross(m_info.front, Vec3::Up());
	if (m_info.right.SqLength() == 0) m_info.right = Vec3::Right();
}
