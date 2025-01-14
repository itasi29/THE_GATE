#include "TurretBullet.h"
#include "Collider/ColliderSphere.h"
#include "Object/Player.h"
#include "EffekseerManager.h"
#include "FileId.h"

namespace
{
	const Vec3 MODLE_SCALE = Vec3(0.5f);

	constexpr float RADIUS = 0.2f;
	constexpr float SPEED = 1.0f;
	constexpr int DAMAGE = 2;
}

TurretBullet::TurretBullet() :
	Object3DBase(Priority::LOW, ObjectTag::TURRET_BULLET),
	m_player(nullptr),
	m_playEffH(-1),
	m_isExist(true)
{
}

TurretBullet::~TurretBullet()
{
}

void TurretBullet::Init(const Vec3& pos, const Vec3& dir, Player* player)
{
//	LoadModel(FILE_BULLET);
	
	OnEntryPhysics();
	m_rigid.SetPos(pos);
	m_rigid.SetVelocity(dir * SPEED);
	m_rigid.SetGravity(false);
	m_rotation = Quaternion::GetQuaternion(Vec3::Back(), dir);
//	m_scale = MODLE_SCALE;

	auto collider = std::dynamic_pointer_cast<MyEngine::ColliderSphere>(CreateCollider(MyEngine::ColKind::SPHERE));
	collider->radius = RADIUS;
	collider->isTrigger = true;

	m_player = player;

	auto& effMgr = EffekseerManager::GetInstance();
	m_playEffH = effMgr.Play(E_BULLET);
}

void TurretBullet::End()
{
	Object3DBase::End();

	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.Stop(m_playEffH);
}

void TurretBullet::Update()
{
	auto& effMgr = EffekseerManager::GetInstance();
	if (!effMgr.IsPlay(m_playEffH))
	{
		m_playEffH = effMgr.Play(E_BULLET);
	}
	effMgr.SetInfo(m_playEffH, m_rigid.GetPos(), m_rotation);
}

void TurretBullet::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// タグがプレイヤーならダメージを与える
	if (tag == ObjectTag::PLAYER)
	{
		m_player->OnDamage(DAMAGE, m_rigid.GetDir());
	}

	// タグがプレイヤー・タレット・タレットの球でないなら球を終了させる
	if (tag != ObjectTag::TURRET && tag != ObjectTag::TURRET_BULLET)
	{
		m_isExist = false;
	}
}
