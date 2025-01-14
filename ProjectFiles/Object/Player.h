#pragma once
#include <unordered_map>
#include <memory>
#include "Object/Object3DBase.h"

class FileBase;
class Camera;
class PlayerCamera;
class GateManager;
class HandObject;
class AnimController;
namespace MyEngine
{
	class ColliderSphere;
	class ColliderCapsule;
	struct CollideHitInfo;
}

class Player : public Object3DBase
{
private:
	enum class State
	{
		IDLE,
		WALK,
		RUN,
		JUMP,
		AERIAL,
		LANDING,
		HIT,
		DEATH,
	};

public:
	Player(const std::shared_ptr<PlayerCamera>& camera, const std::shared_ptr<GateManager>& gateMgr);
	virtual ~Player();

	void AsyncInit();
	void Init(const Vec3& pos, const Vec3& dir);
	void Init(const Vec3& pos, bool isOneHand);
	void Restart(const Vec3& pos);
	void Update() override;
	/// <summary>
	/// 銃の描画
	/// </summary>
	void DrawGun() const;
	/// <summary>
	/// プレイヤー由来のUI描画
	/// </summary>
	void DrawUI() const;
	/// <summary>
	/// ダメージを分かりやすく描画
	/// </summary>
	void DrawDamageFillter() const;

	/// <summary>
	/// ダメージ
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="hitDir">ヒットバック方向</param>
	void OnDamage(int damage, const Vec3& hitDir);

	bool IsDeath() const { return m_isDeath; }
	std::shared_ptr<Camera> GetCamera() const;

	virtual void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	virtual void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	void HandUpdate();
	void GunUpdate();
	void RecoverHpUpdate();
	void HpBarUpdate();
	void RotationUpdate();
	void AnimUpdate();

	void IdleUpdate();
	void WalkUpdate();
	void RunUpdate();
	void JumpUpdate();
	void AerialUpdate();
	void LandingUpdate();
	void HitUpdate();
	void DeathUpdate();

	bool Move(float speed, bool isFixedStick);
	void AerialMove();

	void OnIdle();
	void OnWalk();
	void OnRun();
	void OnJump();
	void OnAerial();
	void OnLanding();
	void OnHit();
	void OnDeath();

	void OnShot();
	void OnHand(MyEngine::Collidable* obj);

	void DrawHpUI() const;
	void DrawPadUI() const;
	void DrawPadUI(int y, bool isActive, int imageId, const wchar_t* const str) const;

private:
	using UpdateFunc_t = void(Player::*)();
	UpdateFunc_t m_updateFunc;

	std::unordered_map<int, std::shared_ptr<FileBase>> m_files;

	std::shared_ptr<PlayerCamera> m_camera;
	std::shared_ptr<GateManager> m_gateMgr;
	std::shared_ptr<AnimController> m_anim;

	std::shared_ptr<MyEngine::ColliderCapsule> m_centerCol;
	std::shared_ptr<MyEngine::ColliderSphere> m_handCol;

	State m_nowState;

	HandObject* m_handObj;

	Quaternion m_nextRot;

	int m_gunVS;
	int m_gunPS;
	int m_hp;
	int m_preHp;
	int m_hitStopFrame;
	int m_receverFrame;
	int m_shotInteval;
	int m_shakeHpBarFrame;
	int m_stayAerialMove;

	bool m_isOneHand;
	bool m_isDeath;
	bool m_isRecever;
	bool m_isGround;
	bool m_isCanCatch;
	bool m_isCatch;
	bool m_isCanWarp;
	bool m_isWarp;

	std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;
};
