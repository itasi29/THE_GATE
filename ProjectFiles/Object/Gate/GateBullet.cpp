#include "GateBullet.h"
#include "GateManager.h"
#include "EffekseerManager.h"
#include "Collider/ColliderSphere.h"
#include "Collider/ColliderBox.h"
#include "FileID.h"

namespace
{
	// FIXME: 後々外部化予定
	constexpr float RADIUS = 0.25f;
	constexpr float SPEED = 1.0f;
}

GateBullet::GateBullet(const std::shared_ptr<GateManager>& gateMgr, GateKind kind) :
	Collidable(Priority::LOW, ObjectTag::GATE_BULLET),
	m_gateMgr(gateMgr),
	m_kind(kind),
	m_effHandle(-1),
	m_isBreak(false)
{
}

GateBullet::~GateBullet()
{
}

void GateBullet::Init(const Vec3& pos, const Vec3& dir)
{
	OnEntryPhysics();
	m_rigid.SetGravity(false);
	m_rigid.SetPos(pos);
	m_rigid.SetVelocity(dir.GetNormalized() * SPEED);
	auto colider = std::dynamic_pointer_cast<MyEngine::ColliderSphere>(CreateCollider(MyEngine::ColKind::SPHERE));
	colider->radius = RADIUS;
	colider->isTrigger = false;

	m_throughTag.push_back(ObjectTag::CAMERA);
	m_throughTag.push_back(ObjectTag::PLAYER);

	auto& effMgr = EffekseerManager::GetInstance();
	if (m_kind == GateKind::Blue)
	{
		m_effHandle = effMgr.Play(E_GATE_BULLET_BLUE);
	}
	else
	{
		m_effHandle = effMgr.Play(E_GATE_BULLET_ORANGE);
	}
}

void GateBullet::End()
{
	Collidable::End();
	EffekseerManager::GetInstance().Stop(m_effHandle);
}

void GateBullet::Update()
{
	// HACK: もしも時間経過でエフェクトが映らなくなったら個々の処理をループできるように変更する
	EffekseerManager::GetInstance().SetInfo(m_effHandle, m_rigid.GetPos(), Quaternion());
}

void GateBullet::OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	Collidable::OnCollideEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	if (tag == ObjectTag::WALL)
	{
		m_gateMgr->CreateGate(m_kind, colider, hitInfo, Vec3::Up());
	}
	else if (tag == ObjectTag::FLOOR || tag == ObjectTag::ROOF)
	{
		auto velDir = m_rigid.GetDir();
		velDir.y = 0;
		m_gateMgr->CreateGate(m_kind, colider, hitInfo, velDir.GetNormalized());
	}

	m_isBreak = true;
}

void GateBullet::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE_DELETE)
	{
		m_isBreak = true;
	}
}
