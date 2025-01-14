#include "GateManager.h"
#include <cassert>
#include "Gate.h"
#include "GateBullet.h"
#include "GateCamera.h"
#include "CameraManager.h"
#include "StageDataManager.h"
#include "Physics.h"
#include "Collider/CollideHelp.h"
#include "Collider/ColliderCapsule.h"
#include "Collider/ColliderBox.h"

namespace
{
	// 判定数
	constexpr int CHECK_NUM = 4;

	// ゲートのカプセルの長さ
	constexpr float GATE_CAPSULE_SIZE = 4.0f;
	// ゲートの半径
	constexpr float GATE_RADIUS = 2.0f;

	const std::list<ObjectTag> CHECK_TAG_LIST =
	{
		ObjectTag::WALL,
		ObjectTag::FLOOR,
		ObjectTag::ROOF,
		ObjectTag::NO_GATE_WALL,
		ObjectTag::NO_GATE_FLOOR,
		ObjectTag::NO_GATE_ROOF,
		ObjectTag::BUTTON,
		ObjectTag::CATWALK,
		ObjectTag::CATWALK_FENCE,
		ObjectTag::CATWALK_PILLAR,
		ObjectTag::DOOR_ARCH,
		ObjectTag::GATE,
		ObjectTag::LASER_CATCHER,
		ObjectTag::LASER_LAUNCH_PAD,
		ObjectTag::PERFORATED_WALL,
		ObjectTag::PISTON,
		ObjectTag::STAIRS,
	};
}

GateManager::GateManager(const std::shared_ptr<CameraManager>& cameraMgr, const wchar_t* const stageName) :
	m_cameraMgr(cameraMgr),
	m_gateOrange(nullptr),
	m_gateBlue(nullptr),
	m_stageName(stageName),
	m_lastShotKind(GateKind::Blue)
{
	m_isCreate[GateKind::Orange] = false;
	m_isCreate[GateKind::Blue] = false;
}

GateManager::~GateManager()
{
}

void GateManager::Init(const std::weak_ptr<Player>& player)
{
	// プレイヤーのセット
	m_player = player;
	// 固定ゲートの設置
	CreateGateOnTerrain();
}

void GateManager::End()
{
	if (m_isCreate.at(GateKind::Orange))
	{
		m_gateOrange->End();
		m_gateOrange = nullptr;
	}
	if (m_isCreate.at(GateKind::Blue))
	{
		m_gateBlue->End();
		m_gateBlue = nullptr;
	}
}

void GateManager::Update()
{
	if (m_isCreate.at(GateKind::Blue)) m_gateBlue->Update();
	if (m_isCreate.at(GateKind::Orange)) m_gateOrange->Update();
	for (auto& bullet : m_bulletList)
	{
		bullet->Update();
	}
	m_bulletList.remove_if(
		[](const auto& bullet)
		{
			if (bullet->IsBreak())
			{
				bullet->End();
				return true;
			}
			return false;
		});
}

void GateManager::DrawGate(int texA, int texB) const
{
	// TODO: 対象のゲートの位置からプレイヤーまでの距離を求めてのカメラに変更する
	if (m_isCreate.at(GateKind::Orange)) m_gateOrange->DrawGate(texA);
	if (m_isCreate.at(GateKind::Blue)) m_gateBlue->DrawGate(texB);
}

void GateManager::Restart()
{
	// ゲートの破棄
	if (m_gateOrange)
	{
		m_gateOrange->End();
		m_gateOrange = nullptr;
	}
	if (m_gateBlue)
	{
		m_gateBlue->End();
		m_gateBlue = nullptr;
	}
	// 未生成に更新
	m_isCreate[GateKind::Orange] = false;
	m_isCreate[GateKind::Blue] = false;
	// ゲート弾を削除
	m_bulletList.clear();
	// 固定ゲートの再設置
	CreateGateOnTerrain();
}

void GateManager::AddBullet(std::shared_ptr<GateBullet> bullet)
{
	m_lastShotKind = bullet->GetKind();
	m_bulletList.emplace_back(bullet);
}

void GateManager::CreateGate(GateKind kind, MyEngine::Collidable* collider, const MyEngine::CollideHitInfo& hitInfo, const Vec3& dir)
{
	Vec3 pos = hitInfo.hitPos;
	auto col = new MyEngine::ColliderCapsule();
	col->dir = dir;
	col->radius = GATE_RADIUS;
	col->size = GATE_CAPSULE_SIZE;

	std::shared_ptr<Gate> gate;
	if (kind == GateKind::Orange)
	{
		gate = m_gateOrange;
	}
	else if (kind == GateKind::Blue)
	{
		gate = m_gateBlue;
	}
	else
	{
		assert(false && "存在しないゲートの種類です");
	}
	
	// TODO: 置いた場所の範囲内で生成させるように変更
	int checkCount = 0;
	MyEngine::Physics::PreHitInfoList_t info;
	info[collider->GetTag()].info = hitInfo;
	info[collider->GetTag()].isCreate = true;
	info[collider->GetTag()].isCheckPallel = true;
	std::list<MyEngine::Collidable*> through{ collider, gate.get() };

	MyEngine::Physics::GetInstance().CheckObject(pos, col, checkCount, CHECK_NUM, true, info, CHECK_TAG_LIST, through);
	delete col;

	// 生成できる場合
	if (checkCount < CHECK_NUM)
	{
		if (kind == GateKind::Orange)
		{
			CreateGate(m_gateOrange, kind, collider->GetTag(), pos, hitInfo.fixDir, dir);
		}
		else if (kind == GateKind::Blue)
		{
			CreateGate(m_gateBlue, kind, collider->GetTag(), pos, hitInfo.fixDir, dir);
		}
	}
	// 生成できない場合
	else
	{
		// TODO: エフェクトと失敗SEを流すようにする
	}
}

std::shared_ptr<Gate> GateManager::GetPairGate(GateKind kind) const
{
	if (kind == GateKind::Orange)
	{
		return m_gateBlue;
	}
	else if (kind == GateKind::Blue)
	{
		return m_gateOrange;
	}
	else
	{
		assert(false && "存在しないゲートの種類です");
		return m_gateOrange;
	}
}

void GateManager::CreateGateOnTerrain()
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	// 片手ステージでないなら終了
	if (!stageDataMgr.IsOneHand(m_stageName)) return;

	// 必要データ取得
	const auto& tag = stageDataMgr.GetGateObjectTag(m_stageName);
	const auto& pos = stageDataMgr.GetGatePos(m_stageName);
	const auto& norm = stageDataMgr.GetGateNorm(m_stageName);
	const auto& dir = stageDataMgr.GetGateDir(m_stageName);
	CreateGate(m_gateBlue, GateKind::Blue, tag, pos, norm, dir);
}

void GateManager::CreateGate(std::shared_ptr<Gate>& gate, GateKind kind, ObjectTag hitTag, const Vec3& pos, const Vec3& norm, const Vec3& dir)
{
	auto pairGate = GetPairGate(kind);
	// ゲートを既に作成している場合
	if (m_isCreate[kind])
	{
		gate->ChangePos(hitTag, pos, norm, dir);
		gate->SetCameraInfo();
	}
	// ゲートをまだ作成していない場合
	else
	{
		CameraKind kind1 = CameraKind::GATE_B;
		CameraKind kind2 = CameraKind::GATE_B_FROM_A;
		if (kind == GateKind::Blue)
		{
			kind1 = CameraKind::GATE_A;
			kind2 = CameraKind::GATE_A_FROM_B;
		}
		gate = std::make_shared<Gate>(std::dynamic_pointer_cast<GateCamera>(m_cameraMgr->GetCamera(kind1)), std::dynamic_pointer_cast<GateCamera>(m_cameraMgr->GetCamera(kind2)), kind);
		gate->Init(hitTag, pos, norm, dir, m_player);
		if (pairGate)
		{
			gate->SetPairGate(pairGate);
			pairGate->SetPairGate(gate);

			gate->SetCameraInfo();
			pairGate->SetCameraInfo();
		}
	}

	m_isCreate[kind] = true;
}
