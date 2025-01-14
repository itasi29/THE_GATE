#pragma once
#include "Object/Object3DBase.h"

class GateManager;

class GateDelete : public Object3DBase
{
public:
	GateDelete(GateManager& gateMgr);
	~GateDelete();

	void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity) override;
	void Update() override;
	void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	void PlayEff(int& handle);

private:
	GateManager& m_gateMgr;

	int m_effH1;
	int m_effH2;
	int m_delayFrame;

	bool m_isDelay;
};

