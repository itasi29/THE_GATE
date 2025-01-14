#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
// 便利系
#include "ScreenUtility.h"
#include "CursorUtility.h"
#include "UIUtility.h"
#include "TimeUtility.h"
#include "MyDraw.h"
// コモン系
#include "Game.h"
#include "Input.h"
#include "CommandList.h"
// マネージャー系
#include "Files.h"
#include "SoundManager.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "FontManager.h"
#include "SceneManager.h"
#include "EffekseerManager.h"
#include "CameraManager.h"
#include "TutorialManager.h"
// その他
#include "SceneStageSelect.h"
#include "SceneResult.h"
#include "Collidable.h"
#include "Camera.h"
#include "PlayerCamera.h"
#include "Object/Object3DBase.h"
#include "Object/Player.h"
#include "Object/Stage/StageManager.h"
#include "Object/Gate/GateManager.h"

namespace
{
	enum class RTKind
	{
		GATE_A_TEX,
		GATE_B_TEX,
		TEMP_1,
		TEMP_2,
		MAX,
	};
	enum class DeathUICurrent : int
	{
		CONTINUE,
		RETURN_STAGE_SELECT,
		MAX,
	};

	// ゲートの色
	constexpr unsigned int COLOR_GATE_BLUE = 0x1376b5;
	constexpr unsigned int COLOR_GATE_ORANGE = 0xf19149;
	// タイムの文字の色
	constexpr unsigned int COLOR_TIME_STR = 0xffffff;
	// タイムバー描画位置
	constexpr int DRAW_TIME_X = 132;
	constexpr int DRAW_TIME_Y = 160;
	// タイムバーの画像サイズ
	constexpr float FILE_SIZE_TIME = 0.5f;
	// タイム文字描画位置
	constexpr int DRAW_TIME_STR_X = 80;
	constexpr int DRAW_TIME_STR_Y = 160;
	// タイム文字のサイズ
	constexpr float TIME_SIZE = 0.25f;
	// タイム文字カラー
	constexpr unsigned int COLOR_TIME = 0xffffff;
	// サイトの画像サイズ
	constexpr float FILE_SIZE_SITE = 0.09375f;
	// 生成ゲートの画像サイズ
	constexpr float FILE_SIZE_CREATE_GATE = 0.375f;
	// 生成ゲートの回転具合
	constexpr float DRAW_CREATE_GATE_ANGLE = 30 * Game::DEG_2_RAD;
	// 生成ゲートの描画位置(青)
	constexpr int DRAW_CREATE_GATE_BLUE_X = 610;
	constexpr int DRAW_CREATE_GATE_BLUE_Y = 330;
	// 生成ゲートの描画位置(オレンジ)
	constexpr int DRAW_CREATE_GATE_ORANGE_X = 670;
	constexpr int DRAW_CREATE_GATE_ORANGE_Y = 390;
	// 最後に発射したゲートの描画位置(青)
	constexpr int DRAW_SHOT_GATE_BLUE_X = 575;
	constexpr int DRAW_SHOT_GATE_BLUE_Y = 368;
	// 最後に発射したゲートの描画位置(オレンジ)
	constexpr int DRAW_SHOT_GATE_ORANGE_X = 705;
	constexpr int DRAW_SHOT_GATE_ORANGE_Y = 352;
	// 最後に発射したゲートの描画半径
	constexpr int SHOT_GATE_RADIUS = 6;
	// 最後に発射したゲートの線の太さ
	constexpr int SHOT_GATE_LINE_THICKNESS = 2;

	// 死亡ウィンドウ文字
	const wchar_t* const STR_DEATH_WINDOW = L"ＹＯＵ　ＤＩＥＤ";
	// 死亡ウィンドウ文字との差
	constexpr int DRAW_DEATH_WINDOW_STR_SUB_Y = 100;
	// 「ＹＯＵ　ＤＩＥＤ」フォントサイズ
	constexpr int FONT_SIZE_DEATH_WINDOW = 48;
	// 「ＹＯＵ　ＤＩＥＤ」文字カラー
	constexpr unsigned int COLOR_DEATH_WINDOW = 0xffffff;
	// 死亡UI文字列
	const std::vector<std::wstring> STR_DEATH =
	{
		L"リスタート",
		L"ステージ選択へ"
	};
	// 死亡UIの描画位置
	constexpr int DRAW_DEATH_UI_FRAME_X = 490;
	constexpr int DRAW_DEATH_UI_FRAME_Y = 450;
	// 死亡UIの描画間隔
	constexpr int DRAW_DEATH_UI_FRAME_X_INTERVL = 300;
	// 死亡UIの画面外位置
	constexpr int DRAW_DEATH_UI_OUT_VAL = 360;
	// 死亡UIのウィンドウ画像サイズ
	constexpr float FILE_SIZE_DEATH_UI_WINDOW = 0.5f;
	// 死亡UIの画像サイズ
	constexpr float FILE_SIZE_DEATH_UI = 0.75f;
	// 死亡UIフォントサイズ
	constexpr int FONT_SIZE_DEATH_UI = 32;
	// 選択文字カラー
	constexpr unsigned int COLOR_SELECT = 0x000000;
	// 未選択文字カラー
	constexpr unsigned int COLOR_NOT_SELECT = 0xffffff;
	// 死亡UIのウェーブサイズ
	constexpr int WAVE_SIZE_DEATH_UI = 16;
	// 死亡UIのウェーブスピード
	constexpr float WAVE_SPEED_DEATH_UI = 2.0f;
}

SceneMain::SceneMain(const wchar_t* const stageName) :
	SceneBase(SceneKind::MAIN),
	m_updateFunc(&SceneMain::MainUpdate),
	m_stageName(stageName),
	m_time(0),
	m_current(0),
	m_count(0)
{
}

void SceneMain::AsyncInit()
{
	// ファイルの読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[PS_GATE_POS]				= fileMgr.Load(PS_GATE_POS);
	m_files[I_TIME_BAR]					= fileMgr.Load(I_TIME_BAR);
	m_files[I_LOOK_SITE]				= fileMgr.Load(I_LOOK_SITE);
	m_files[I_CREATE_GATE_ORANGE]		= fileMgr.Load(I_CREATE_GATE_ORANGE);
	m_files[I_CREATE_GATE_BLUE]			= fileMgr.Load(I_CREATE_GATE_BLUE);
	m_files[I_NOT_CREATE_GATE_ORANGE]	= fileMgr.Load(I_NOT_CREATE_GATE_ORANGE);
	m_files[I_NOT_CREATE_GATE_BLUE]		= fileMgr.Load(I_NOT_CREATE_GATE_BLUE);
	m_files[I_OPTION_WINDOW]			= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_COMMON_FRAME]				= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_COMMON_SELECT_FRAME]		= fileMgr.Load(I_COMMON_SELECT_FRAME);
	m_files[B_MAIN]						= fileMgr.Load(B_MAIN);
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.LoadFile(E_BULLET);
	effMgr.LoadFile(E_GATE_BULLET_BLUE);
	effMgr.LoadFile(E_GATE_BULLET_ORANGE);
	effMgr.LoadFile(E_DEATH_TURRET);
	effMgr.LoadFile(E_MUZZLE_FLASH);
	effMgr.LoadFile(E_GATE_DELETE);
	effMgr.LoadFile(E_LASER_BULLET);

	// マネージャー生成
	m_stageMgr = std::make_shared<StageManager>(m_stageName);
	m_cameraMgr = std::make_shared<CameraManager>();
	m_gateMgr = std::make_shared<GateManager>(m_cameraMgr, m_stageName);
	// プレイヤー生成
	m_player = std::make_shared<Player>(std::dynamic_pointer_cast<PlayerCamera>(m_cameraMgr->GetCamera(CameraKind::PLAYER)), m_gateMgr);
	// 初期化(非同期)
	m_stageMgr->AsyncInit();
	m_player->AsyncInit();
}

void SceneMain::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_MAIN)->GetHandle());
	// 画面生成
	m_rtTable.resize(static_cast<int>(RTKind::MAX));
	for (auto& handle : m_rtTable) handle = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
	// UI生成
	m_deathUIWindow = UIMoveData::Make(Game::CENTER_X, -DRAW_DEATH_UI_OUT_VAL);
	m_deathUIList = UIMoveData::Make(static_cast<int>(DeathUICurrent::MAX), DRAW_DEATH_UI_FRAME_X, -DRAW_DEATH_UI_OUT_VAL, DRAW_DEATH_UI_FRAME_X_INTERVL, 0);
	// 初期化(同期)
	m_stageMgr->Init(m_player.get(), m_gateMgr.get());
	m_gateMgr->Init(m_player);
	SoundManager::GetInstance().SetSeCenter(m_player);
	m_player->Init(m_stageMgr->GetCheckPoint(), StageDataManager::GetInstance().IsOneHand(m_stageName));
}

void SceneMain::End()
{
	m_gateMgr->End();
	m_stageMgr->End();
	m_cameraMgr->End();
	m_player->End();

	for (auto& handle : m_rtTable) DeleteGraph(handle);
	EffekseerManager::GetInstance().ReleaseFile();
	SoundManager::GetInstance().ResetSeCenter();
}

void SceneMain::Update(bool isFade)
{
	auto& stageDataMgr = StageDataManager::GetInstance();
#ifdef _DEBUG
	printf("StageNo:%d\n", stageDataMgr.GetStageNo(m_stageName));
#endif
	++m_count;
	m_deathUIWindow->Update();
	for (auto& data : m_deathUIList) data->Update();

	if (isFade) return;
	// フェードが終了したフレームの場合
	if (m_scnMgr.IsFadeEndFrame())
	{
		// "チュートリアルステージ"かつ"クリアしていない"ならチュートリアルを行う
		const bool isTutorial = stageDataMgr.IsTutorial(m_stageName);
		const bool isNotClear = !SaveDataManager::GetInstance().IsClear(m_stageName);
		if (isTutorial && isNotClear)
		{
			TutorialManager::GetInstance().Start(stageDataMgr.GetTutorialId(m_stageName));
		}
	}
	(this->*m_updateFunc)();
}

void SceneMain::Draw() const
{
	// モデル描画：ゲートの生成状況に合わせて変更
	if (m_gateMgr->IsCreateBothGates()) DrawBlend();
	else								DrawNormal();
	// UI描画
	SetUseZBuffer3D(false);
	DrawCommonUI();
	DrawExistUI();
	m_player->DrawDamageFillter();
	DrawDeathUI();
	SetUseZBuffer3D(true);
}

void SceneMain::OnRestart() const
{
	// 各種リスタート処理
	m_player->Restart(m_stageMgr->GetCheckPoint());
	m_gateMgr->Restart();
	m_stageMgr->Restart();
}

void SceneMain::MainUpdate()
{
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::MAIN, this);
		return;
	}
	// 各種更新
	m_gateMgr->Update();
	m_stageMgr->Update();
	m_player->Update();
	m_cameraMgr->Update();
	SaveDataManager::GetInstance().UpdateTime(m_stageName);
	// タイム更新
	++m_time;
#ifdef _DEBUG
	if (input.IsTriggerd(KEY_INPUT_K))
	{
		m_player->OnDamage(99999, Vec3());
	}
	if (input.IsTriggerd(KEY_INPUT_J))
	{
		m_player->OnDamage(10, Vec3());
	}
#endif
	if (m_stageMgr->CheckClear())
	{
		SaveDataManager::GetInstance().OnClear(m_stageName, m_time);
		auto next = std::make_shared<SceneResult>(m_stageName, m_time);
		m_scnMgr.Change(next);
		return;
	}
	// 死亡したら更新先変更
	if (m_player->IsDeath())
	{
		m_updateFunc = &SceneMain::GameOverUpdate;
		UIMoveData::ChangeVertical(m_deathUIList, DRAW_DEATH_UI_FRAME_Y);
		m_deathUIWindow->ChangeVertical(Game::CENTER_Y);
		m_count = 0;
	}
}

void SceneMain::GameOverUpdate()
{
	if (CursorUtility::CursorUp<DeathUICurrent>(m_current, DeathUICurrent::MAX, Command::BTN_LEFT))		m_count = 0;
	if (CursorUtility::CursorDown<DeathUICurrent>(m_current, DeathUICurrent::MAX, Command::BTN_RIGHT))	m_count = 0;

	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OK))
	{
		if (m_current == static_cast<int>(DeathUICurrent::CONTINUE))
		{
			OnRestart();
			m_updateFunc = &SceneMain::MainUpdate;
			UIMoveData::ChangeVertical(m_deathUIList, -DRAW_DEATH_UI_OUT_VAL);
			m_deathUIWindow->ChangeVertical(-DRAW_DEATH_UI_OUT_VAL);
		}
		else if (m_current == static_cast<int>(DeathUICurrent::RETURN_STAGE_SELECT))
		{
			auto next = std::make_shared<SceneStageSelect>();
			m_scnMgr.Change(next);
		}
		else
		{
			assert(false);
		}
	}

	// 各種更新(見た目だけ更新)
	m_gateMgr->Update();
	m_stageMgr->Update();
	m_player->Update();
	m_cameraMgr->Update();
}

void SceneMain::DrawNormal() const
{
	// プレイヤーから見た景色をそのまま描画
	DrawModelBlend(DX_SCREEN_BACK, -1, -1, CameraKind::PLAYER, m_cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps());
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.SyncCamera();
	effMgr.Draw();
}

void SceneMain::DrawBlend() const
{
	auto rtGateA = m_rtTable[static_cast<int>(RTKind::GATE_A_TEX)];
	auto rtGateB = m_rtTable[static_cast<int>(RTKind::GATE_B_TEX)];
	
	// それぞれのゲートから見た景色をゲートに張り付けて描画する
	DrawGateBlend(rtGateA, CameraKind::GATE_A, CameraKind::GATE_A_FROM_B);
	DrawGateBlend(rtGateB, CameraKind::GATE_B, CameraKind::GATE_B_FROM_A);

	// ゲート内の景色を張り付けてプレイヤーから見た景色を描画
	DrawModelBlend(DX_SCREEN_BACK, rtGateA, rtGateB, CameraKind::PLAYER, m_cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps());
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.SyncCamera();
	effMgr.Draw();
}

void SceneMain::DrawGateBlend(int rt, CameraKind gate, CameraKind gateFrom) const
{
	auto rtTemp1 = m_rtTable[static_cast<int>(RTKind::TEMP_1)];
	auto rtTemp2 = m_rtTable[static_cast<int>(RTKind::TEMP_2)];

	// ゲートからゲートを見た景色の描画
	{
		SetDrawScreen(rtTemp1);
		ClearDrawScreen();
		m_cameraMgr->AppInfo(gateFrom);

		m_stageMgr->Draw();
		m_player->Draw();
		m_player->DrawGun();
	}
	// ゲート内にゲートがある場合の描画を行う
	// MEMO: 1度では不十分な可能性があるので3回は行っておく
	for (int i = 0; i < 3; ++i)
	{
		SetDrawScreen(rtTemp2);
		m_cameraMgr->AppInfo(gateFrom);
		if (i == 0)
		{
			ClearDrawScreen();
			m_gateMgr->DrawGate(rtTemp1, rtTemp1);
		}
		else
		{
			m_gateMgr->DrawGate(rtTemp2, rtTemp2);
		}

		m_stageMgr->Draw();
		m_player->Draw();
		m_player->DrawGun();
	}
	// ゲートからの画面を描画してあげる
	DrawModelBlend(rt, rtTemp2, rtTemp2, gate, true);
}

void SceneMain::DrawModelBlend(int rt, int tex1, int tex2, CameraKind camera, bool isPlayerDraw) const
{
	SetDrawScreen(rt);
	ClearDrawScreen();
	m_cameraMgr->AppInfo(camera);

	m_gateMgr->DrawGate(tex1, tex2);

	m_stageMgr->Draw();
	if (isPlayerDraw) m_player->Draw();
	m_player->DrawGun();
}

void SceneMain::DrawCommonUI() const
{
	// 時間描画
	DrawRotaGraphFast(DRAW_TIME_X, DRAW_TIME_Y, FILE_SIZE_TIME, 0.0f, m_files.at(I_TIME_BAR)->GetHandle(), true);
	TimeUtility::DrawTime(DRAW_TIME_STR_X, DRAW_TIME_STR_Y, m_time, TIME_SIZE, COLOR_TIME);
	// プレイヤー情報描画
	m_player->DrawUI();
}

void SceneMain::DrawExistUI() const
{
	// プレイヤーが死んでいる場合は処理終了
	if (m_player->IsDeath()) return;

	// サイトの描画
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, FILE_SIZE_SITE, 0.0f, m_files.at(I_LOOK_SITE)->GetHandle(), true);

	int handle;
	// 青ゲート
	if (m_gateMgr->IsCreate(GateKind::Blue))	handle = m_files.at(I_CREATE_GATE_BLUE)->GetHandle();
	else										handle = m_files.at(I_NOT_CREATE_GATE_BLUE)->GetHandle();
	DrawRotaGraphFast(DRAW_CREATE_GATE_BLUE_X, DRAW_CREATE_GATE_BLUE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);
	// オレンジゲート
	if (m_gateMgr->IsCreate(GateKind::Orange))	handle = m_files.at(I_CREATE_GATE_ORANGE)->GetHandle();
	else										handle = m_files.at(I_NOT_CREATE_GATE_ORANGE)->GetHandle();
	DrawRotaGraphFast(DRAW_CREATE_GATE_ORANGE_X, DRAW_CREATE_GATE_ORANGE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);

	int x = DRAW_SHOT_GATE_BLUE_X;
	int y = DRAW_SHOT_GATE_BLUE_Y;
	unsigned int color = COLOR_GATE_BLUE;
	GateKind kind = GateKind::Blue;
	if (m_gateMgr->GetLastShotKind() == GateKind::Orange)
	{
		x = DRAW_SHOT_GATE_ORANGE_X;
		y = DRAW_SHOT_GATE_ORANGE_Y;
		color = COLOR_GATE_ORANGE;
		kind = GateKind::Orange;
	}
	// HACK: 何故かLineThicknessを変更した際に塗りつぶしができない為2個重ねている
	DrawCircle(x, y, SHOT_GATE_RADIUS, color, false, SHOT_GATE_LINE_THICKNESS);
	if (m_gateMgr->IsCreate(kind))  DrawCircle(x, y, SHOT_GATE_RADIUS, color, true);
}

void SceneMain::DrawDeathUI() const
{
	// プレイヤーが生きている場合は処理終了
	if (!m_player->IsDeath()) return;

	// 画像ハンドル
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	// 描画
	DrawRotaGraphFast(m_deathUIWindow->x, m_deathUIWindow->y, FILE_SIZE_DEATH_UI_WINDOW, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	int x = UIUtility::GetSubCenterFromLen(m_deathUIWindow->x, STR_DEATH_WINDOW, FONT_SIZE_DEATH_WINDOW);
	DrawStringToHandle(x, m_deathUIWindow->y - DRAW_DEATH_WINDOW_STR_SUB_Y, STR_DEATH_WINDOW, COLOR_DEATH_WINDOW, FontManager::GetInstance().GetHandle(FONT_SIZE_DEATH_WINDOW));
	UIUtility::DrawFrameAndStr(m_deathUIList, FILE_SIZE_DEATH_UI, 0.0f, FONT_SIZE_DEATH_UI, m_current, STR_DEATH, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, WAVE_SIZE_DEATH_UI, m_count * WAVE_SPEED_DEATH_UI);
}
