#pragma once
#include "Object/Object3DBase.h"
#include <memory>
#include <unordered_map>

class Player;
class GateManager;
class Gate;

class HandObject : public Object3DBase
{
private:
	// MEMO: もしfriendを使わずに済むならそちらの処理に変更する
	friend Player;

public:
	HandObject(GateManager& gateMgr);
	~HandObject();

	void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity) override;
	void Update() override;
	void Restart() override;

	void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	void AppModelInfo() const override;

	void OnHnad();
	void EndHand();

	void SetPos(const Vec3& pos);
	void OnPlayerWarp();

private:
	GateManager& m_gateMgr;
	Gate* m_gate;

	// 初期位置
	Vec3 m_initPos;
	// 移動前の位置
	Vec3 m_prePos;
	// ワープ時の位置
	Vec3 m_warpPos;

	// 判定サイズ
	float m_colSize;

	// 
	std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;
	// 
	bool m_isOneHand;
	// 
	bool m_isWarp;
	// 
	bool m_isCanWarp;
};
