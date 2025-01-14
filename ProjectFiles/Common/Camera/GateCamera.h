#pragma once
#include "Camera.h"

class Gate;

class GateCamera : public Camera
{
public:
	GateCamera();
	virtual ~GateCamera();

	void Update(const CameraInfo& playerCamera, const Vec3& pairBase);

	void SetBaseViewDir(const Vec3& dir) { m_baseViewDir = dir; }

private:
	Vec3 m_baseViewDir;
};

