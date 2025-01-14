#pragma once
#include "Object/Object3DBase.h"

namespace MyEngine
{
	struct CollideHitInfo;
}
class GateManager;
class LaserLaunchPad;

class LaserBullet : public Object3DBase
{
public:
	LaserBullet(LaserLaunchPad* launcPad, const GateManager* gateMgr, const int reflectNum);
	~LaserBullet();

	void Init(const Vec3& pos, const Vec3& dir);
	void End() override;
	void Update() override;

	virtual void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	const GateManager* m_gateMgr;
	const int m_maxReflectNum;

	LaserLaunchPad* m_launchPad;

	int m_effHandle;
	int m_reflectNum;

	bool m_isCanWarp;
	bool m_isWarp;
	bool m_isPreWarp;
	std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;
};

