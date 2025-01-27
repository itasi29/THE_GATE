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

enum class PlayerState
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

class Player : public Object3DBase
{
public:
	Player(const std::shared_ptr<PlayerCamera>& camera, const std::shared_ptr<GateManager>& gateMgr);
	virtual ~Player();

	/// <summary>
	/// 非同期初期化処理
	/// </summary>
	void AsyncInit();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pos">初期位置</param>
	/// <param name="dir">初期向き</param>
	/// <param name="isOneHand">片手ステージかどうか</param>
	void Init(const Vec3& pos, const Vec3& dir, bool isOneHand);

	/// <summary>
	/// リスタート処理
	/// </summary>
	/// <param name="pos">スポーン位置</param>
	/// <param name="dir">向いている方向</param>
	void Restart(const Vec3& pos, const Vec3& dir);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 銃の描画
	/// </summary>
	void DrawGun() const;
	/// <summary>
	/// HP描画
	/// </summary>
	void DrawHpUI() const;
	/// <summary>
	/// Pad情報描画
	/// </summary>
	void DrawPadUI() const;
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
	/// <summary>
	/// プレイヤーを停止させる(速度を0にする)
	/// </summary>
	void OnStop();
	/// <summary>
	/// プレイヤーを指定の座標までワープさせる
	/// </summary>
	/// <param name="pos">座標</param>
	void OnWarpPos(const Vec3& pos);

	/// <summary>
	/// 死亡判定
	/// </summary>
	/// <returns>true: 死亡/ false: 生存</returns>
	bool IsDeath() const { return m_isDeath; }
	/// <summary>
	/// カメラを取得
	/// </summary>
	/// <returns>プレイヤーカメラ</returns>
	std::shared_ptr<Camera> GetCamera() const;
	/// <summary>
	/// 現在のステートを取得する
	/// </summary>
	/// <returns>現在のステート</returns>
	PlayerState GetNowState() const { return m_nowState; }

	/// <summary>
	/// 衝突開始時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// 衝突中の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー侵入時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー中の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー退出時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	void HandUpdate();
	void GunUpdate();
	void RecoverHpUpdate();
	void HpBarUpdate();
	void RotationUpdate();
	void AnimUpdate();
	void CameraUpdate();

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

	PlayerState m_nowState;

	HandObject* m_handObj;

	Quaternion m_nextRot;

	Vec3 m_gatePos;

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
	bool m_isChangeNear;

	std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;
};
