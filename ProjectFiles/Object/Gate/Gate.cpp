#include "Gate.h"
#include "Collider/ColliderCapsule.h"
#include "Physics.h"
#include "Files.h"
#include "SoundManager.h"
#include "Matrix4x4.h"
#include "MathHelp.h"
#include "Game.h"
#include "GateCamera.h"
#include "Object/Player.h"
#include "DebugDraw.h"

namespace
{
	// カプセルの長さ
	constexpr float CAPSULE_SIZE = 4.0f;
	// 半径
	constexpr float RADIUS = 2.0f;
}

Gate::Gate(const std::shared_ptr<GateCamera>& camera, const std::shared_ptr<GateCamera>& cameraFromPair, GateKind kind) :
	Object3DBase(Priority::STATIC, ObjectTag::GATE),
	m_updateFunc(&Gate::NormalUpdate),
	m_camera(camera),
	m_cameraFromPair(cameraFromPair),
	m_kind(kind),
	m_hitObjTag(ObjectTag::WALL)
{
	m_cbuffH = CreateShaderConstantBuffer(sizeof(UserData));
	m_userData = static_cast<UserData*>(GetBufferShaderConstantBuffer(m_cbuffH));
}

Gate::~Gate()
{
}

void Gate::Init(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir, const std::weak_ptr<Player>& player)
{
	OnEntryPhysics();

	auto& fileMgr = FileManager::GetInstance();
	m_dissolve = fileMgr.Load(I_DISSOLVE);
	m_warpSe = fileMgr.Load(S_WARP);

	m_rigid.SetGravity(false);
	m_collider = std::dynamic_pointer_cast<MyEngine::ColliderCapsule>(CreateCollider(MyEngine::ColKind::CAPSULE));
	m_collider->isTrigger = true;
	m_collider->size = CAPSULE_SIZE;
	m_collider->radius = RADIUS;
	int id = M_GATE_ORANGE;
	if (m_kind == GateKind::Blue)
	{
		id = M_GATE_BLUE;
	}
	LoadModel(id);
	m_scale *= 0.01f * CAPSULE_SIZE;

	m_player = player;

	ChangePos(hitObjTag, pos, norm, dir);
}

void Gate::End()
{
	Object3DBase::End();
	m_pairGate.reset();
	m_pairGate = nullptr;
}

void Gate::ChangePos(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir)
{
	m_hitObjTag = hitObjTag;
	m_norm = norm.GetNormalized();
	m_rigid.SetPos(pos);
	m_collider->dir = dir;
	m_right = Vec3::Cross(m_collider->dir, m_norm);
	m_updateFunc = &Gate::OpenUpdate;

	m_rotation = Quaternion::GetQuaternion(m_norm, Vec3::Front());
	// 壁に設置していない場合
	if (m_collider->dir.y < 1.0f)
	{
		m_rotation = Quaternion::GetQuaternion(Vec3::Front(), m_collider->dir) * m_rotation;
	}

	m_userData->center = pos;
	m_userData->dir = dir;
	m_userData->size = CAPSULE_SIZE;
	m_userData->radius = RADIUS * 0.95f;
	m_userData->frame = 0;
}

void Gate::SetCameraInfo()
{
	if (!m_pairGate) return;

	const auto& pos = m_rigid.GetPos();

	m_camera->SetTargetPos(pos);
	m_cameraFromPair->SetTargetPos(pos);

	// 見る方向は法線方向
	m_camera->SetBaseViewDir(m_norm);
	// 見る方向は自身からペアの方向に向かうベクトルを法線方向に回転させて実行
	auto dir = m_pairGate->GetPos() - pos;
	dir.Normalize();
	auto angle = std::acosf(Vec3::Dot(-m_norm, m_pairGate->GetNorm())) * Game::RAD_2_DEG;
	auto axis = Vec3::Cross(m_pairGate->GetNorm(), -m_norm);
	if (!axis.SqLength())
	{
		axis = m_collider->dir;
	}
	m_cameraFromPair->SetBaseViewDir(Quaternion::AngleAxis(angle, axis) * dir);
}

void Gate::Update()
{
#ifdef _DEBUG
	printf("---------ゲートカメラ情報-----------\n");
	if (m_kind == GateKind::Orange)
	{
		printf("ゲート：オレンジ\n");
	}
	else
	{
		printf("ゲート：ブルー\n");
	}
	const auto& pos = m_camera->GetInfo().targetPos;
	printf("座標：(%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z);
	const auto& dir = m_camera->GetInfo().look;
	printf("方向(法線)：(%.2f, %.2f, %.2f)\n", dir.x, dir.y, dir.z);
	const auto& dir2 = m_cameraFromPair->GetInfo().look;
	printf("方向(対象)：(%.2f, %.2f, %.2f)\n", dir2.x, dir2.y, dir2.z);
#endif


	// シェーダーに送る情報更新
	++m_userData->frame;
	m_userData->isCreate = m_pairGate != nullptr;

	// ペアのゲートが存在しない場合終了
	if (!m_pairGate) return;

	// カメラの向き更新
	const auto& pairNorm = m_pairGate->GetNorm();
	const auto& playerCameraInfo = m_player.lock()->GetCamera()->GetInfo();
	m_camera->Update(playerCameraInfo, pairNorm);
	m_cameraFromPair->Update(playerCameraInfo, pairNorm);
}

void Gate::DrawGate(int tex) const
{
	UpdateShaderConstantBuffer(m_cbuffH);
	SetShaderConstantBuffer(m_cbuffH, DX_SHADERTYPE_PIXEL, 4);

	if (tex < 0)
	{
		tex = MV1GetTextureGraphHandle(m_modelHandle, 0);
	}
	SetUseTextureToShader(1, tex);
	SetUseTextureToShader(2, m_dissolve->GetHandle());
	Object3DBase::Draw();
	SetUseTextureToShader(1, -1);
	SetUseTextureToShader(2, -1);

#ifdef _DEBUG
	auto& debug = MyEngine::DebugDraw::GetInstance();
	debug.DrawLine(m_rigid.GetPos(), m_rigid.GetPos() + m_norm * 5, 0x00ff00);
#endif
}

bool Gate::CheckWarp(const Vec3& targetPos)
{
	// ゲートから対象に向くベクトルとゲートの法線の内積が-になったらワープ可能
	const auto& gateTotarget = targetPos - (m_rigid.GetPos() + m_collider->center);
	auto dot = Vec3::Dot(m_norm, gateTotarget);
	auto a = dot * Game::RAD_2_DEG;
	return dot < 0.0f;
}

void Gate::OnWarp(const Vec3& targetPos, MyEngine::Rigidbody& targetRigid, bool isAddGravity)
{
	// 音鳴らす
	SoundManager::GetInstance().PlaySe(m_warpSe->GetHandle());


	/* 場所変換 */
	const auto& gateToTarget = targetPos - m_rigid.GetPos();

	const auto& normRot = Quaternion::GetQuaternion(m_norm, m_pairGate->GetNorm());
	const auto& upRot = Quaternion::GetQuaternion(m_collider->dir, m_pairGate->GetCol()->dir);
	// ゲート上での右方向・上方向動いている大きさを取得
	const auto& right = normRot * Vec3::Projection(gateToTarget, m_right);
	const auto& up    = upRot * Vec3::Projection(gateToTarget, m_collider->dir);
	// 修正位置の取得
	auto fixPos = m_pairGate->GetPos() + right + up + m_pairGate->GetNorm() * 0.1f;
	targetRigid.SetPos(fixPos);
	

	/* 速度変換 */
	// 現在の速度を取得
	auto vel = targetRigid.GetVelocity();
	auto velLen = vel.Length();
	vel.Normalize();

	auto rot1 = Quaternion::GetQuaternion(-m_norm, m_pairGate->GetNorm(), m_collider->dir);
	auto newVel = rot1 * vel * velLen;
	targetRigid.SetVelocity(newVel);
}

void Gate::OpenUpdate()
{
}

void Gate::NormalUpdate()
{
}

void Gate::CloseUpdate()
{
}
