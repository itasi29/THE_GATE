#include "Player.h"
#include "RandomUtility.h"
#include "UIUtility.h"
#include "Game.h"
#include "Files.h"
#include "AnimPlayer.h"
#include "MathHelp.h"
#include "Easing.h"
#include "Input.h"
#include "CommandList.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "AnimController.h"
#include "PlayerCamera.h"
#include "Physics.h"
#include "Matrix4x4.h"
#include "Collider/ColliderSphere.h"
#include "Collider/ColliderCapsule.h"
#include "Gate/GateManager.h"
#include "Gate/GateBullet.h"
#include "Gate/Gate.h"
#include "Object/HandObject.h"
#include "Gimmick/Turret/Turret.h"
#include "DebugDraw.h"

namespace
{
	enum class ColIndex
	{
		CENTER,
		MODEL,
	};

	/* 判定系 */
	constexpr float MODEL_COL_RADIUS = 1.0f;	// モデルカプセル半径
	constexpr float MODEL_COL_SIZE = 3.5f;		// モデルカプセルサイズ
	constexpr float CENTER_COL_RADIUS = 0.5f;	// 中心カプセル半径
	constexpr float CENTER_COL_SIZE = 0.6f;		// 中心カプセルサイズ
	constexpr float CENTER_COL_SIZE_UP = 8.0f;	// 増加中心カプセルサイズ
	constexpr float HAND_RADIUS = 2.0f;	// 物を持てる範囲
	constexpr float HAND_DIS = 4.0f;	// 物を持つ当たり判定の中心との距離
	/* ステータス系 */
	constexpr float WALK_SPEED = 0.15f;	// 歩き移動速度
	constexpr float RUN_SPEED = 0.35f;	// 走り移動速度
	constexpr float JUMP_POWER = 0.55f;	// ジャンプ力
	constexpr int MAX_HP = 100;			// 最大HP

	/* フレーム関係 */
	constexpr int SHOT_INTERVAL = 60;			// 撃つ間隔
	constexpr float SHOT_RECOIL_FRAME = 45.0f;	// 銃の反動
	constexpr int RECEVER_WAIT_FRAME = 120;		// HP回復までの待機フレーム
	constexpr int STAY_AERIAL_MOVE_FRAME = 6;	// 空中で移動するまでの待機フレーム

	/* モデル */
	const wchar_t* const ANIM_INFO_PATH = L"Data/Master/AnimPlayer.csv";	// アニメーションパス
	constexpr float MODEL_SIZE_SCALE = 0.0325f;								//モデルのファイルサイズ
	// ピポット
	const Vec3 LOOK_PIVOT = Vec3(0, MODEL_COL_SIZE * 0.5f, 0);						// 見る場所
	const Vec3 MODEL_PIVOT = Vec3(0, -MODEL_COL_RADIUS - MODEL_COL_SIZE * 0.5f, 0);	// モデル場所

	/* 銃モデル関係 */
	const wchar_t* const RIGHT_HAND_FRAME = L"mixamorig:RightHand";			// 右手のリグフレーム名
	const Quaternion GUN_ROT = Quaternion::AngleAxis(-10.0f, Vec3::Up());	// 銃ベース回転
	// 銃モデルサイズ
	constexpr float GUN_SIZE_TPS = 0.0025f;
	constexpr float GUN_SIZE_FPS = 0.00025f;
	// 銃モデルの前後左右描画位置
	constexpr float GUN_FRONT_RATE_TPS = 0.75f;
	constexpr float GUN_UP_RATE_TPS = 0.4f;
	constexpr float GUN_FRONT_RATE_FPS = 0.08f;
	constexpr float GUN_UP_RATE_FPS = 0.05f;
	constexpr float GUN_RIGHT_RATE_FPS = 0.14f;

	/* HPUI系 */
	constexpr float FILE_SIZE = 0.35f;		// 画像サイズ
	constexpr int DRAW_HP_FRME_X = 157;		// フレームX描画位置
	constexpr int DRAW_HP_FRME_Y = 70;		// フレームY描画位置
	constexpr int DRAW_HP_BAR_X = 157 + 36;	// バーX描画位置
	constexpr int DRAW_HP_BAR_Y = 70;		// バーY描画位置
	constexpr int SHAKE_SIZE = 5;			// 揺らす大きさ
	constexpr int SHKE_HP_BAR_FRAME = 30;				// HPバーを揺らすフレーム
	constexpr int START_DECREASE_PRE_HP_BAR_FRAME = 10;	// 減少HPを減らし始めるまでのフレーム

	/* PadUI系 */
	constexpr int DRAW_PAD_Y = 680;			// ベースY描画位置
	constexpr int DRAW_PAD_Y_INTERVAL = 40;	// Y描画間隔
	constexpr int DRAW_PAD_X = 1080;		// 画像X描画位置
	constexpr float FILE_SIZE_PAD = 0.3f;	// サイズ
	constexpr int DRAW_PAD_STR_X = 1100;	// 文字列X描画位置
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int PAD_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_ACTIVE = 0xffffff;		// 有効文字カラー
	constexpr unsigned int COLOR_NOT_ACTIVE = 0x888888;	// 無効文字カラー
}

Player::Player(const std::shared_ptr<PlayerCamera>& camera, const std::shared_ptr<GateManager>& gateMgr) :
	Object3DBase(Priority::HIGH, ObjectTag::PLAYER),
	m_updateFunc(&Player::IdleUpdate),
	m_camera(camera),
	m_gateMgr(gateMgr),
	m_handObj(nullptr),
	m_nowState(State::IDLE),
	m_gunPS(-1),
	m_gunVS(-1),
	m_hp(MAX_HP),
	m_preHp(MAX_HP),
	m_hitStopFrame(0),
	m_receverFrame(0),
	m_shotInteval(0),
	m_shakeHpBarFrame(-1),
	m_stayAerialMove(0),
	m_isOneHand(false),
	m_isDeath(false),
	m_isRecever(false),
	m_isGround(false),
	m_isCanCatch(false),
	m_isCatch(false),
	m_isCanWarp(false),
	m_isWarp(false)
{
}

Player::~Player()
{
}

void Player::AsyncInit()
{
	// ファイル読み込み
	LoadModel(M_PLAYER);
	auto& fileMgr = FileManager::GetInstance();
	m_files[M_GUN] = fileMgr.Load(M_GUN);
	m_files[I_HP_FRAME] = fileMgr.Load(I_HP_FRAME);
	m_files[I_BASE_HP_BAR] = fileMgr.Load(I_BASE_HP_BAR);
	m_files[I_NOW_HP_BAR] = fileMgr.Load(I_NOW_HP_BAR);
	m_files[I_HIT_HP_BAR] = fileMgr.Load(I_HIT_HP_BAR);
	m_files[I_DAMAGE_FILLTER] = fileMgr.Load(I_DAMAGE_FILLTER);
	m_files[I_PAD_A] = fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B] = fileMgr.Load(I_PAD_B);
	m_files[I_PAD_X] = fileMgr.Load(I_PAD_X);
	m_files[I_PAD_LT] = fileMgr.Load(I_PAD_LT);
	m_files[I_PAD_RT] = fileMgr.Load(I_PAD_RT);
	m_files[I_PAD_RB] = fileMgr.Load(I_PAD_RB);
	m_files[I_COMMON_FRAME] = fileMgr.Load(I_COMMON_FRAME);
	m_files[S_LANDING] = fileMgr.Load(S_LANDING);
	m_files[S_WALK] = fileMgr.Load(S_WALK);
	m_files[S_SHOT_PLAYER] = fileMgr.Load(S_SHOT_PLAYER);
	m_files[S_DAMAGE] = fileMgr.Load(S_DAMAGE);

	m_gunVS = fileMgr.GetVS(M_GUN);
	m_gunPS = fileMgr.GetPS(M_GUN);
}

void Player::Init(const Vec3& pos, const Vec3& dir)
{
	m_rigid.SetPos(pos);
	m_camera->SetTargetPos(pos);
	m_camera->SetFront(dir, 0.0f);
}

void Player::Init(const Vec3& pos, bool isOneHand)
{
	m_anim = std::make_shared<AnimController>();
	m_anim->Init(ANIM_INFO_PATH, m_modelHandle, ANIM_IDLE);

	OnEntryPhysics();
	m_rigid.SetPos(pos + Vec3(0, 1, 0));
	m_rigid.SetGravity(true);
	m_updateFunc = &Player::IdleUpdate;
	m_pivot = MODEL_PIVOT;
	m_scale *= MODEL_SIZE_SCALE;

	m_centerCol = std::dynamic_pointer_cast<MyEngine::ColliderCapsule>(CreateCollider(MyEngine::ColKind::CAPSULE));
	m_centerCol->isTrigger = true;
	m_centerCol->radius = CENTER_COL_RADIUS;
	m_centerCol->size = CENTER_COL_SIZE;
	m_centerCol->dir = Vec3::Up();
	auto col2 = std::dynamic_pointer_cast<MyEngine::ColliderCapsule>(CreateCollider(MyEngine::ColKind::CAPSULE));
	col2->radius = MODEL_COL_RADIUS;
	col2->size = MODEL_COL_SIZE;
	col2->dir = Vec3::Up();

	m_handCol = std::make_shared<MyEngine::ColliderSphere>();
	m_handCol->radius = HAND_RADIUS;
	m_handCol->isTrigger = true;

	m_isOneHand = isOneHand;

	m_hp = MAX_HP;
	m_preHp = MAX_HP;

	m_shakeHpBarFrame = -1;
}

void Player::Restart(const Vec3& pos)
{
	// アニメーション初期化
	m_anim->Change(ANIM_IDLE, true, false, false);

	// 位置変更
	m_rigid.SetPos(pos);
	// 速度初期化
	m_rigid.SetVelocity(Vec3());

	// ものを持っていれば離す
	if (m_isCatch)
	{
		m_handObj->EndHand();
		m_handCol = nullptr;
		m_isCatch = false;
	}

	// 体力系初期化
	m_hp = MAX_HP;
	m_preHp = MAX_HP;
	m_shakeHpBarFrame = -1;

	// 更新先変更
	m_updateFunc = &Player::IdleUpdate;
	// 死亡していないことに
	m_isDeath = false;
}

void Player::Update()
{
	HandUpdate();
	GunUpdate();
	RecoverHpUpdate();
	HpBarUpdate();
	(this->*m_updateFunc)();
	RotationUpdate();
	AnimUpdate();

	m_centerCol->size = m_rigid.GetVelocity().y * CENTER_COL_SIZE_UP + CENTER_COL_SIZE;

	m_camera->Update(m_rigid.GetPos() + LOOK_PIVOT);
}

void Player::DrawGun() const
{
	if (m_isDeath) return;

	auto& cInfo = m_camera->GetInfo();
	// 三人称モードでなければモデル情報を適用化
	if (!cInfo.isTps) AppModelInfo();

	SetUseVertexShader(m_gunVS);
	SetUsePixelShader(m_gunPS);

	auto index = MV1SearchFrame(m_modelHandle, RIGHT_HAND_FRAME);



	auto rot = m_rotation * GUN_ROT;
	Vec3 pos;
	Vec3 scale;

	// 三人称モード
	if (cInfo.isTps)
	{
		const auto& back = rot * Vec3::Back();
		pos = Vec3(MV1GetFramePosition(m_modelHandle, index)) + back * GUN_FRONT_RATE_TPS + Vec3::Up() * GUN_UP_RATE_TPS;
		scale = Vec3(GUN_SIZE_TPS);
	}
	// 一人称モード
	else
	{
		rot = cInfo.vertexRot * rot;
		auto vec = cInfo.right * GUN_RIGHT_RATE_FPS + cInfo.front * GUN_FRONT_RATE_FPS - Vec3::Up() * GUN_UP_RATE_FPS;
		if (m_shotInteval > (SHOT_INTERVAL - SHOT_RECOIL_FRAME))
		{
			float rate = (m_shotInteval - (SHOT_INTERVAL - SHOT_RECOIL_FRAME)) / SHOT_RECOIL_FRAME;
			vec -= cInfo.front * GUN_FRONT_RATE_FPS * 0.5f * rate;
		}
		pos = cInfo.targetPos + cInfo.vertexRot * vec;
		scale = Vec3(GUN_SIZE_FPS);
	}

	auto mat = Matrix4x4::Scale(scale) * Matrix4x4::Rot(rot) * Matrix4x4::Pos(pos);

	const auto handle = m_files.at(M_GUN)->GetHandle();
	MV1SetMatrix(handle, mat.GetMATRIX());
	MV1DrawModel(handle);
}

void Player::DrawUI() const
{
	DrawHpUI();
	DrawPadUI();
}

void Player::DrawDamageFillter() const
{
	float rate = 1.0f - m_hp / static_cast<float>(MAX_HP);
	int blend = static_cast<int>(255 * rate);
	SetDrawBlendMode(DX_BLENDMODE_ADD, blend);
	DrawRotaGraph(Game::CENTER_X, Game::CENTER_Y, 10.0f, 0.0f, m_files.at(I_DAMAGE_FILLTER)->GetHandle(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Player::OnDamage(int damage, const Vec3& hitDir)
{
	if (m_isDeath) return;

	// 減少HPが現在のHP未満なら代入する
	if (m_preHp < m_hp)
	{
		m_preHp = m_hp;
	}
	m_shakeHpBarFrame = SHKE_HP_BAR_FRAME;

	m_hp -= damage;
	if (m_hp <= 0)
	{
		OnDeath();
		m_hp = 0;
		return;
	}
	m_receverFrame = RECEVER_WAIT_FRAME;
	m_isRecever = true;

	constexpr float HIT_BACK_POWER = 0.2f;

	auto vel = hitDir * HIT_BACK_POWER;
	vel.y = m_rigid.GetVelocity().y;
	m_rigid.SetVelocity(vel);

	OnHit();
}

std::shared_ptr<Camera> Player::GetCamera() const
{
	return m_camera;
}

void Player::OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::TURRET)
	{
		auto turret = dynamic_cast<Turret*>(colider);
		turret->OnDamage(m_rigid.GetDir());
	}
}

void Player::OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideStay(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	const auto isFind = std::find(m_groundTag.begin(), m_groundTag.end(), tag) != m_groundTag.end();
	if (isFind)
	{
		OnLanding();
		if (tag == ObjectTag::FLOOR_MOVE)
		{
			auto vel = m_rigid.GetVelocity();
			vel.y = 0;
			m_rigid.SetVelocity(vel);
			auto pos = m_rigid.GetPos();
			pos += colider->GetVelocity();
			m_rigid.SetPos(pos);
		}
	}
}

void Player::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		if (!m_gateMgr->IsCreateBothGates()) return;

		if (selfIndex == static_cast<int>(ColIndex::MODEL))
		{
			// 入ったときに既にワープできる場合は壁裏から入っているのでワープしないようにする
			auto gate = dynamic_cast<Gate*>(colider);
			m_isCanWarp = !gate->CheckWarp(m_rigid.GetPos());
		}
	}
}

void Player::OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerStay(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		if (!m_isCanWarp || !m_gateMgr->IsCreateBothGates()) return;

		auto gate = dynamic_cast<Gate*>(colider);
		// 中心の判定の場合
		if (selfIndex == static_cast<int>(ColIndex::CENTER))
		{
			if (!m_isAddTag[colider])
			{
				// スルータグ追加
				auto gate = dynamic_cast<Gate*>(colider);
				m_throughTag.emplace_back(gate->GetHitObjectTag());
				m_isAddTag[colider] = true;
			}
		}
		// モデルの判定の場合
		else if (selfIndex == static_cast<int>(ColIndex::MODEL))
		{
			// ワープ判定
			if (gate->CheckWarp(m_rigid.GetPos()))
			{
				gate->OnWarp(m_rigid.GetPos(), m_rigid, true);

				auto pairGate = gate->GetPairGate();
				m_camera->OnWarp(-gate->GetNorm(), pairGate->GetNorm(), m_rigid.GetPos());
				OnAerial();
				if (m_handObj) m_handObj->OnPlayerWarp();

				// スルータグの変更
				if (m_isAddTag[colider]) m_throughTag.pop_back();
				m_isAddTag[colider] = false;
				m_throughTag.push_back(pairGate->GetHitObjectTag());
				m_isAddTag[pairGate.get()] = true;
			}
		}
	}
}

void Player::OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerExit(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE && selfIndex == static_cast<int>(ColIndex::CENTER))
	{
		if (m_isAddTag[colider])
		{
			// スルータグ外す
			auto gate = dynamic_cast<Gate*>(colider);
			auto hitTag = gate->GetHitObjectTag();
			for (auto it = m_throughTag.begin(); it != m_throughTag.end(); ++it)
			{
				if (*it == hitTag)
				{
					m_throughTag.erase(it);
					break;
				}
			}
			m_isAddTag[colider] = false;
		}
	}
}

void Player::HandUpdate()
{
	if (m_isDeath) return;

	// 持つ位置に持てるオブジェクトがいるか調べる
	m_handCol->center = m_camera->GetInfo().look * HAND_DIS + LOOK_PIVOT;
	auto& phsyics = MyEngine::Physics::GetInstance();
	auto pos = m_rigid.GetPos();
	int count = 0;
	auto res = phsyics.CheckObject(pos, m_handCol.get(), count, 1, false, {}, { ObjectTag::HAND_OBJ });
	// 持てるオブジェクトがある場合
	m_isCanCatch = !res.empty();
	if (m_isCanCatch)
	{
		auto& input = Input::GetInstance();
		// オブジェクトを持つor離す
		if (input.IsTriggerd(Command::BTN_HAND))
		{
			OnHand(res[0].col);
		}
	}

	// オブジェクトを持っている場合
	if (m_isCatch)
	{
		// オブジェクトの位置を持つ位置に更新
		m_handObj->SetPos(m_rigid.GetPos() + m_handCol->center);
	}

#ifdef _DEBUG
	auto& debug = MyEngine::DebugDraw::GetInstance();
	debug.DrawSphere(m_rigid.GetPos() + m_handCol->center, m_handCol->radius, 0x0000ff, false);
#endif
}

void Player::GunUpdate()
{
	--m_shotInteval;
	// トリガーが入力されたら発射
	const auto& trigger = Input::GetInstance().GetTriggerData();
	if (trigger.LT > 0.0f || trigger.RT > 0.0f)
	{
		OnShot();
	}
}

void Player::RecoverHpUpdate()
{
	if (m_isDeath) return;

	// 回復しきっていたら処理なし
	if (!m_isRecever) return;

	// 回復待機時間のを減らす
	--m_receverFrame;
	// 回復待機中なら終了
	if (m_receverFrame > 0) return;

	// HP増加
	++m_hp;
	// 最大HPを超えたら修正して回復終了とする
	if (m_hp > MAX_HP)
	{
		m_hp = MAX_HP;
		m_isRecever = false;
	}
}

void Player::HpBarUpdate()
{
	// 揺らし時間内なら時間を減らしていく
	if (m_shakeHpBarFrame > -1)
	{
		--m_shakeHpBarFrame;
	}

	// 減少HPが現在のHPより大きく、揺らし時間が一定時間経っていれば減少HPを減らしていく
	if (m_preHp > m_hp && m_shakeHpBarFrame < SHKE_HP_BAR_FRAME - START_DECREASE_PRE_HP_BAR_FRAME)
	{
		--m_preHp;
		// HPより小さくなったらHPと同じに修正する
		if (m_preHp < m_hp)
		{
			m_preHp = m_hp;
		}
	}
}

void Player::RotationUpdate()
{
	const auto& cInfo = m_camera->GetInfo();
	if (cInfo.isTps)
	{
		m_rotation = Easing::Slerp(m_rotation, m_nextRot, 0.2f);
	}
	else
	{
		m_nextRot = Quaternion::GetQuaternion(Vec3::Back(), cInfo.front);
		m_rotation = m_nextRot;
	}
}

void Player::AnimUpdate()
{
	if (m_nowState == State::WALK)
	{
		auto sqSpeed = m_rigid.GetVelocity().SqLength();
		auto rate = (sqSpeed / (WALK_SPEED * WALK_SPEED)) * 0.7f + 0.3f;
		m_anim->Update(rate);
	}
	else
	{
		m_anim->Update();
	}
}

void Player::IdleUpdate()
{
	auto& input = Input::GetInstance();
	const auto& trigger = input.GetTriggerData();
	// 左スティックが入力されたら移動状態に遷移
	if (trigger.LStick.SqLength() > 0.0f)
	{
		OnWalk();
		return;
	}
	// ジャンプに遷移
	if (input.IsTriggerd(Command::BTN_JUMP))
	{
		OnJump();
		return;
	}
}

void Player::WalkUpdate()
{
	auto& input = Input::GetInstance();
	// ジャンプに遷移
	if (input.IsTriggerd(Command::BTN_JUMP))
	{
		OnJump();
		SoundManager::GetInstance().Stop(m_files.at(S_WALK)->GetHandle());
		return;
	}
	// 走りに遷移
	if (input.IsTriggerd(Command::BTN_RUN))
	{
		OnRun();
		SoundManager::GetInstance().Stop(m_files.at(S_WALK)->GetHandle());
		return;
	}
	// 左スティックが入力されている間は移動
	if (Move(WALK_SPEED, false)) return;

	// 入力されていなかったらアイドル状態に遷移
	OnIdle();
	SoundManager::GetInstance().Stop(m_files.at(S_WALK)->GetHandle());
}

void Player::RunUpdate()
{
	auto& input = Input::GetInstance();
	// ジャンプに遷移
	if (input.IsTriggerd(Command::BTN_JUMP))
	{
		OnJump();
		//		SoundManager::GetInstance().Stop(m_files.at(S_WALK)->GetHandle());
		return;
	}
	// 歩きに遷移
	if (input.IsReleased(Command::BTN_RUN))
	{
		OnWalk();
		return;
	}
	// 左スティックが入力されている間は移動
	if (Move(RUN_SPEED, true)) return;

	// 入力されていなかったらアイドル状態に遷移
	OnIdle();
	//	SoundManager::GetInstance().Stop(m_files.at(S_WALK)->GetHandle());
}

void Player::JumpUpdate()
{
	if (m_anim->IsLoop())
	{
		OnAerial();
		return;
	}
	--m_stayAerialMove;
	if (m_stayAerialMove < 0)
	{
		AerialMove();
	}
}

void Player::AerialUpdate()
{
	AerialMove();
}

void Player::LandingUpdate()
{
	auto& input = Input::GetInstance();
	const auto& trigger = input.GetTriggerData();
	// 左スティックが入力されたら移動状態に遷移
	if (trigger.LStick.SqLength() > 0.0f)
	{
		OnWalk();
	}
	// 何もせずにアニメーションがループ完了したら待機状態に遷移
	else if (m_anim->IsLoop())
	{
		OnIdle();
	}
}

void Player::HitUpdate()
{
	// ヒットストップ中はどこにも遷移しない
	--m_hitStopFrame;
	if (m_hitStopFrame > 0) return;

	auto& input = Input::GetInstance();
	const auto& trigger = input.GetTriggerData();
	// 左スティックが入力されたら移動状態に遷移
	if (trigger.LStick.SqLength() > 0.0f)
	{
		OnWalk();
	}
	// 何もせずにアニメーションがループ完了したら待機状態に遷移
	else if (m_anim->IsLoop())
	{
		OnIdle();
	}
}

void Player::DeathUpdate()
{
	// 特になし
}

bool Player::Move(float speed, bool isFixedStick)
{
	const auto& trigger = Input::GetInstance().GetTriggerData();
	// 移動していなければ終了
	if (trigger.LStick.SqLength() <= 0.0f) return false;

	// カメラの向いている方向に合わせて移動ベクトルを作成
	auto& cInfo = m_camera->GetInfo();
	Vec3 newVel = cInfo.front * trigger.LStick.y + cInfo.right * trigger.LStick.x;
	// スティックの向きを固定する場合は正規化
	if (isFixedStick) newVel.Normalize();
	// 移動ベクトルを移動スピードの長さに変換して、y軸を現在の速度にする
	newVel *= speed;
	newVel.y = m_rigid.GetVelocity().y;
	m_rigid.SetVelocity(newVel);

	// モデルの向きを更新
	if (cInfo.isTps) m_nextRot = Quaternion::GetQuaternion(Vec3::Back(), newVel);

	return true;
}

void Player::AerialMove()
{
	const auto& trigger = Input::GetInstance().GetTriggerData();
	// 移動していないければ終了
	if (trigger.LStick.SqLength() <= 0.0f) return;

	constexpr float AERIAL_MOVE_POWER = 0.008f;

	// カメラの向いている方向に合わせて移動ベクトルを作成
	auto& cInfo = m_camera->GetInfo();
	Vec3 dir = cInfo.front * trigger.LStick.y + cInfo.right * trigger.LStick.x;
	dir.GetNormalized();
	// 現在の速度をY軸を別で保存して取得
	auto nowVel = m_rigid.GetVelocity();
	float temp = nowVel.y;
	nowVel.y = 0;
	// 方向に合わせて速度を変更
	auto newVel = nowVel + dir * AERIAL_MOVE_POWER;
	// 歩きの速度より大きい場合
	bool isUpdate = true;
	if (newVel.SqLength() > WALK_SPEED * WALK_SPEED)
	{
		// 新しい速度が現在の速度より小さいなら更新
		isUpdate = nowVel.SqLength() > newVel.SqLength();
	}
	// 更新
	if (isUpdate)
	{
		newVel.y = temp;
		m_rigid.SetVelocity(newVel);
	}

	// モデルの向きを更新
	if (cInfo.isTps) m_nextRot = Quaternion::GetQuaternion(Vec3::Back(), dir);
}

void Player::OnIdle()
{
	// 速度ゼロ
	m_rigid.SetVelocity(Vec3());
	m_anim->Change(ANIM_IDLE);
	m_updateFunc = &Player::IdleUpdate;
	m_nowState = State::IDLE;
}

void Player::OnWalk()
{
	SoundManager::GetInstance().PlaySe3D(m_files.at(S_WALK)->GetHandle(), shared_from_this());
	m_anim->Change(ANIM_WALK);
	m_updateFunc = &Player::WalkUpdate;
	m_nowState = State::WALK;
}

void Player::OnRun()
{
	//	SoundManager::GetInstance().PlaySe3D(m_files.at(S_WALK)->GetHandle(), shared_from_this());
	m_anim->Change(ANIM_RUN);
	m_updateFunc = &Player::RunUpdate;
	m_nowState = State::RUN;
}

void Player::OnJump()
{
	if (!m_isGround) return;

	auto vel = m_rigid.GetVelocity();
	vel.y = JUMP_POWER;
	m_rigid.SetVelocity(vel);
	m_stayAerialMove = STAY_AERIAL_MOVE_FRAME;
	m_isGround = false;
	m_anim->Change(ANIM_JUMP, true, true);
	m_updateFunc = &Player::JumpUpdate;
	m_nowState = State::JUMP;
}

void Player::OnAerial()
{
	m_isGround = false;
	m_anim->Change(ANIM_AERIAL);
	m_updateFunc = &Player::AerialUpdate;
	m_nowState = State::AERIAL;
}

void Player::OnLanding()
{
	if (m_isGround) return;

	SoundManager::GetInstance().PlaySe3D(m_files.at(S_LANDING)->GetHandle(), shared_from_this());
	m_isGround = true;
	// 重量停止状態でないなら速度を0にする
	if (m_rigid.GetStayGravityFrame() < 0)
	{
		m_rigid.SetVelocity(Vec3());
	}
	m_anim->Change(ANIM_LANDING, true, true);
	m_updateFunc = &Player::LandingUpdate;
	m_nowState = State::LANDING;
}

void Player::OnHit()
{
	constexpr int HIT_STOP_FRAME = 3;
	m_hitStopFrame = HIT_STOP_FRAME;

	SoundManager::GetInstance().PlaySe3D(m_files.at(S_DAMAGE)->GetHandle(), shared_from_this());
	m_anim->Change(ANIM_HIT, true, true, false);
	m_updateFunc = &Player::HitUpdate;
	m_nowState = State::HIT;
}

void Player::OnDeath()
{
	if (m_isDeath) return;

	m_isDeath = true;
	m_rigid.SetVelocity(Vec3());
	m_anim->Change(ANIM_DEATH, true, true);
	m_updateFunc = &Player::DeathUpdate;
	m_nowState = State::DEATH;
}

void Player::OnShot()
{
	// ショット待機時間なら撃たない
	if (m_shotInteval > 0) return;

	auto& input = Input::GetInstance();
	const auto& trigger = input.GetTriggerData();
	bool isCreate = false;
	GateKind kind;
	// 右トリガーが押されていたらオレンジゲートを撃つ
	if (trigger.RT > 0.0f)
	{
		isCreate = true;
		kind = GateKind::Orange;
	}
	// 片手ステージではないかつ左トリガーが押されていたらブルーゲートを撃つ
	else if (!m_isOneHand && trigger.LT > 0.0f)
	{
		isCreate = true;
		kind = GateKind::Blue;
	}
	// 弾の生成
	if (isCreate)
	{
		SoundManager::GetInstance().PlaySe3D(m_files.at(S_SHOT_PLAYER)->GetHandle(), shared_from_this());
		// インターバルを初期化
		m_shotInteval = SHOT_INTERVAL;
		// 生成
		auto bullet = std::make_shared<GateBullet>(m_gateMgr, kind);
		// カメラの向いている方向に弾を進ませる
		bullet->Init(m_rigid.GetPos() + LOOK_PIVOT, m_camera->GetInfo().look);
		// 追加
		m_gateMgr->AddBullet(bullet);
	}
}

void Player::OnHand(MyEngine::Collidable* obj)
{
	if (m_isCatch)
	{
		m_handObj->EndHand();
		m_handObj = nullptr;
	}
	else
	{
		m_handObj = dynamic_cast<HandObject*>(obj);
		m_handObj->OnHnad();
	}

	m_isCatch = !m_isCatch;
}

void Player::DrawHpUI() const
{
	// 画像サイズ取得
	int w, h;
	GetGraphSize(m_files.at(I_BASE_HP_BAR)->GetHandle(), &w, &h);
	// 大きさ変更
	w = static_cast<int>(w * FILE_SIZE);
	h = static_cast<int>(h * FILE_SIZE);

	int shakeX = 0;
	int shakeY = 0;
	// バーを揺らす場合
	if (m_shakeHpBarFrame > 0)
	{
		// サイズ調整
		int size = static_cast<int>((static_cast<float>(m_shakeHpBarFrame) / SHKE_HP_BAR_FRAME) * SHAKE_SIZE);
		// 揺らす範囲をランダムで取得
		auto& rand = Random::GetInstance();
		shakeX = rand.GetRand(0, size);
		shakeY = rand.GetRand(0, size);
	}

	// フレームの描画
	DrawRotaGraphFast(DRAW_HP_FRME_X + shakeX, DRAW_HP_FRME_Y + shakeY, FILE_SIZE, 0.0f, m_files.at(I_HP_FRAME)->GetHandle(), true);
	// ベースとなるHPバーを描画
	DrawRotaGraphFast(DRAW_HP_BAR_X + shakeX, DRAW_HP_BAR_Y + shakeY, FILE_SIZE, 0.0f, m_files.at(I_BASE_HP_BAR)->GetHandle(), true);
	// 下二つのバーの描画位置を取得
	int x = DRAW_HP_BAR_X - static_cast<int>(w * 0.5f) + shakeX;
	int y = DRAW_HP_BAR_Y - static_cast<int>(h * 0.5f) + shakeY;
	// 減少HPの割合を取得
	float rate = m_preHp / static_cast<float>(MAX_HP);
	// 減少HPバーを割合を考慮して描画
	DrawExtendGraph(x, y, x + static_cast<int>(w * rate), y + h, m_files.at(I_HIT_HP_BAR)->GetHandle(), true);
	// 現在HPの割合を取得
	rate = m_hp / static_cast<float>(MAX_HP);
	// 現在HPバーを割合を考慮して描画
	DrawExtendGraph(x, y, x + static_cast<int>(w * rate), y + h, m_files.at(I_NOW_HP_BAR)->GetHandle(), true);
}

void Player::DrawPadUI() const
{
	// ダッシュ
	int y = DRAW_PAD_Y;
	DrawPadUI(y, true, I_PAD_X, L"ダッシュ");
	// ジャンプ
	y -= DRAW_PAD_Y_INTERVAL;
	DrawPadUI(y, m_isGround, I_PAD_A, L"ジャンプ");
	// 物持つ
	y -= DRAW_PAD_Y_INTERVAL;
	if (m_isCatch)	DrawPadUI(y, m_isCanCatch, I_PAD_B, L"離す");
	else			DrawPadUI(y, m_isCanCatch, I_PAD_B, L"掴む");
	// 左ゲート
	const bool isShot = m_shotInteval <= 0;
	y -= DRAW_PAD_Y_INTERVAL;
	DrawPadUI(y, isShot, I_PAD_RT, L"弾発射(オレンジ)");
	// 右ゲート
	y -= DRAW_PAD_Y_INTERVAL;
	DrawPadUI(y, isShot && !m_isOneHand, I_PAD_LT, L"弾発射(ブルー)");
}

void Player::DrawPadUI(int y, bool isActive, int imageId, const wchar_t* const str) const
{
	const int fontH = FontManager::GetInstance().GetHandle(FONT_SIZE_PAD);
	unsigned int color;
	if (isActive)	color = COLOR_ACTIVE;
	else			color = COLOR_NOT_ACTIVE;

	// 画像
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(imageId)->GetHandle(), true);
	// 文字列
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, color, str, FONT_SIZE_PAD, 0x000000);
}
