#include "Turret.h"
#include "Easing.h"
#include "Matrix4x4.h"
#include "MathHelp.h"
#include "Files.h"
#include "AnimController.h"
#include "EffekseerManager.h"
#include "TurretBullet.h"
#include "Object/Player.h"
#include "Collider/ColliderSphere.h"
#include "AnimTurret.h"

namespace
{
	// フレーム関係
	const wchar_t* const FRAME_RIGHT_WING = L"RT_Wing";
	const wchar_t* const FRAME_LEFT_WING = L"LFT_Wing";

	// アニメーションデータパス
	const wchar_t* const ANIM_INFO_PATH = L"Data/Master/AnimTurret.csv";
	// 死亡アニメーションフレーム
	constexpr int DEATH_ANIM_FRAME = 120;
	constexpr float DEATH_ROT_ANGLE = 90.0f / DEATH_ANIM_FRAME;

	const Vec3 MODEL_SCALE = Vec3(0.0325f);
	const Vec3 MODEL_PIVOT = Vec3(0, -0.5f, 0);

	constexpr float RADIUS = 3.0f;

	// 見つける範囲
	constexpr float DISCORVERY_ANGLE = 60.0f;
	constexpr float DISCOVERY_RANGE = 1.0f - DISCORVERY_ANGLE / 180.0f;
	// 見つける距離
	constexpr float DISCOVERY_LEN = 50.0f;
	// 
	constexpr float LOOK_SPEED = 1.5f;

	// 攻撃範囲
	constexpr float ATTACK_RANGE = 1.0f - 10.0f / 180.0f;
	// 攻撃間隔
	constexpr float ATTACK_INTERVAL = 10;
}

Turret::Turret() :
	Object3DBase(Priority::LOW, ObjectTag::TURRET),
	m_updateFunc(nullptr),
	m_player(nullptr),
	m_state(State::OPEN),
	m_attackFrame(0),
	m_deathEffPlayH(-1),
	m_rightWingH(-1),
	m_leftWingH(-1),
	m_isCreateLeft(true)
{
}

Turret::~Turret()
{
}

void Turret::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	auto& fileMgr = FileManager::GetInstance();
	m_deathEff = fileMgr.Load(E_DEATH_TURRET);

	Object3DBase::Init(pos, MODEL_SCALE, rot, list, isGravity);
	m_pivot = MODEL_PIVOT;
	m_rightWingH = MV1SearchFrame(m_modelHandle, FRAME_RIGHT_WING);
	m_leftWingH = MV1SearchFrame(m_modelHandle, FRAME_LEFT_WING);

	m_anim = std::make_shared<AnimController>();
	m_anim->Init(ANIM_INFO_PATH, m_modelHandle, ANIM_OPEN);
}

void Turret::Init(const Vec3& dir, Player* player)
{
	m_baseDir = dir;
	m_lookDir = dir;

	m_player = player;

	OnSearch();
}

void Turret::End()
{
	Object3DBase::End();
	for (auto& item : m_bulletList) item->End();
}

void Turret::Update()
{
	// 自身の更新
	(this->*m_updateFunc)();
	AnimUpdate();
	// 弾の更新
	for (auto& item : m_bulletList) item->Update();
	m_bulletList.remove_if(
		[](const auto& bullet)
		{
			if (!bullet->IsExist())
			{
				bullet->End();
				return true;
			}
			return false;
		}
	);
}

void Turret::Draw() const
{
	Object3DBase::Draw();
	for (auto& item : m_bulletList) item->Draw();

#ifdef _DEBUG
	auto s = m_rigid.GetPos() + m_collider.at(0)->center;
	auto e = s + m_baseDir * 3;
	DrawLine3D(s.VGet(), e.VGet(), 0x0000ff);
	e = s + m_lookDir * 3;
	DrawLine3D(s.VGet(), e.VGet(), 0x00ff00);
#endif
}

void Turret::OnDamage(const Vec3& dir)
{
	m_updateFunc = &Turret::DeathUpdate;
	m_state = State::DEATH;
	m_anim->Change(ANIM_ROTATION, true, false, true, false);
	auto& effMgr = EffekseerManager::GetInstance();
	m_deathEffPlayH = effMgr.Play(E_DEATH_TURRET);
	effMgr.SetInfo(m_deathEffPlayH, m_rigid.GetPos(), m_rotation);

	m_nextDir = m_lookDir;
	
	const auto& rot = Quaternion::GetQuaternion(Vec3::Up(), dir);
	m_fallRot = Quaternion::AngleAxis(DEATH_ROT_ANGLE, rot.GetAxis());
}

void Turret::AnimUpdate()
{
	const auto& axis = Vec3::Cross(m_lookDir, m_baseDir);
	const auto& dot = Vec2::Dot(m_lookDir.GetXZDir(), m_baseDir.GetXZDir());
	float rate = (1.0f - dot) / (1.0f - DISCOVERY_RANGE);
	if (std::signbit(axis.y)) rate *= -1;
	rate = (rate + 1) * 0.5f;
	m_anim->Update(1.0f, rate);
}

void Turret::SearchUpdate()
{
	// 発見したらオープン状態に遷移
	if (IsSearch())
	{
		OnOpen();
	}

	// 向いてい方向をベース方向に直していく
	m_lookDir = Easing::AngleRotation(m_lookDir, m_baseDir, LOOK_SPEED);
}

void Turret::OpenUpdate()
{
	if (m_anim->IsLoop())
	{
		OnAttack();
	}
}

void Turret::AttackUpdate()
{
	// 見失ったらクローズ状態に遷移
	if (!IsSearch())
	{
		OnClose();
		return;
	}

	/* 回転処理 */
	const auto& turretPos = m_rigid.GetPos() + m_collider.at(0)->center;
	const auto& toPlayer = m_player->GetPos() - turretPos;
	m_lookDir = Easing::AngleRotation(m_lookDir, toPlayer.GetNormalized(), LOOK_SPEED);

	/* 攻撃処理 */
	++m_attackFrame;
	if (m_attackFrame > ATTACK_INTERVAL)
	{
		m_attackFrame = 0;
		// 弾生成
		Shot();
	}
}

void Turret::CloseUpdate()
{
	if (m_anim->IsLoop())
	{
		OnSearch();
	}
}

void Turret::DeathUpdate()
{
	// エフェクトが終了したら壊す
	auto& effMgr = EffekseerManager::GetInstance();
	if (!effMgr.IsPlay(m_deathEffPlayH))
	{
		m_isExist = false;
		return;
	}

	// 回転処理
	m_rotation = m_fallRot * m_rotation;
	m_baseDir = m_fallRot * m_baseDir;
	m_lookDir = Easing::AngleRotation(m_lookDir, m_nextDir, LOOK_SPEED);

	// エフェクトが終了するまで最後のあがきをする
	++m_attackFrame;
	if (m_attackFrame > ATTACK_INTERVAL)
	{
		// 方向をランダムに変更
		m_nextDir = GetRandomVec(m_baseDir, DISCORVERY_ANGLE);

		m_attackFrame = 0;
		// 弾生成
		Shot();
	}
}

bool Turret::IsSearch() const
{
	// プレイヤーまでの距離が遠ければ見つけていない
	const auto& toPlayer = m_player->GetPos() - (m_rigid.GetPos() + m_collider.at(0)->center);
	if (toPlayer.SqLength() > DISCOVERY_LEN * DISCOVERY_LEN) return false;
	// ベースとなるベクトルとプレイヤーまでのベクトルの内積が一定値以上なら見つけている
	const auto& dot = Vec3::Dot(toPlayer.GetNormalized(), m_baseDir);
	return dot > DISCOVERY_RANGE;
}

void Turret::Shot()
{
	int wingIndex;
	float bit = 1;
	// 交互にWingの位置から生成
	if (m_isCreateLeft) 
	{
		m_isCreateLeft = false;
		wingIndex = m_leftWingH;
		bit = -1;
	}
	else
	{
		m_isCreateLeft = true;
		wingIndex = m_rightWingH;
	}
	Vec3 pos = MV1GetFramePosition(m_modelHandle, wingIndex);
	const auto vec = Vec3::Cross(Vec3::Up(), m_lookDir);
	pos = pos + m_lookDir * 0.5f + vec * 0.5f * bit;

	auto bullet = std::make_shared<TurretBullet>();
	bullet->Init(pos, m_lookDir, m_player);
	m_bulletList.emplace_back(bullet);

	auto& effMgr = EffekseerManager::GetInstance();
	int handle = effMgr.Play(E_MUZZLE_FLASH);
	effMgr.SetInfo(handle, pos, m_rotation);
}

void Turret::OnSearch()
{
	m_updateFunc = &Turret::SearchUpdate;
	m_state = State::SEARCH;
}

void Turret::OnOpen()
{
	m_updateFunc = &Turret::OpenUpdate;
	m_state = State::OPEN;
	m_anim->Change(ANIM_OPEN, true, true);
}

void Turret::OnAttack()
{
	m_updateFunc = &Turret::AttackUpdate;
	m_state = State::ATTACK;
	m_anim->Change(ANIM_ROTATION, true, false, true);
}

void Turret::OnClose()
{
	m_updateFunc = &Turret::CloseUpdate;
	m_state = State::CLOSE;
	m_anim->Change(ANIM_CLOSE, true, true);
}
