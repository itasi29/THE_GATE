#pragma once
#include "Object/Object3DBase.h"
#include <list>
#include <memory>

class AnimController;
class FileBase;
class Player;
class TurretBullet;

class Turret : public Object3DBase
{
private:
	enum class State
	{
		SEARCH,
		OPEN,
		ATTACK,
		CLOSE,
		DEATH,
	};

public:
	Turret();
	~Turret();

	void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity) override;
	void Init(const Vec3& dir, Player* player);
	void End() override;
	void Update() override;
	void Draw() const override;

	void OnDamage(const Vec3& dir);
//	void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	void AnimUpdate();

	void SearchUpdate();
	void OpenUpdate();
	void AttackUpdate();
	void CloseUpdate();
	void DeathUpdate();

	bool IsSearch() const;
	void Shot();

	void OnSearch();
	void OnOpen();
	void OnAttack();
	void OnClose();
//	void OnDeath();

	
private:
	using UpdateFunc_t = void(Turret::*)();
	UpdateFunc_t m_updateFunc;

	std::shared_ptr<AnimController> m_anim;
	std::shared_ptr<FileBase> m_deathEff;

	std::list<std::shared_ptr<TurretBullet>> m_bulletList;

	Player* m_player;

	State m_state;

	Vec3 m_baseDir;
	Vec3 m_lookDir;
	Vec3 m_nextDir;

	Quaternion m_fallRot;

	int m_attackFrame;
	int m_deathEffPlayH;
	int m_rightWingH;
	int m_leftWingH;

	bool m_isCreateLeft;
};

