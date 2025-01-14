#include "SceneTitle.h"
#include <DxLib.h>
#include <vector>
#include <sstream>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "TimeUtility.h"
#include "Game.h"
#include "Files.h"
#include "Application.h"
#include "FontManager.h"
#include "SceneManager.h"
#include "SceneStageSelect.h"
#include "SaveDataManager.h"
#include "StageDataManager.h"
#include "RankingDataManager.h"
#include "SoundManager.h"
#include "EffekseerManager.h"
#include "Camera/PlayerCamera.h"
#include "Camera/CameraManager.h"
#include "Object/Player.h"
#include "Object/Gate/GateManager.h"
#include "Object/Stage/StageManager.h"
#include "Object/Gate/GateBullet.h"

namespace
{
	// メニュー選択
	enum class MenuCurrent : int
	{
		SELECT_SAVE,
		RANKING,
		OPOTION,
		END,
		MAX
	};
	// データ確認
	enum class DecideCurrent : int
	{
		CONTINU,
		DELETE_DATA,
		MAX
	};
	// 削除確認
	enum class CheckCurrent : int
	{
		CANCEL,
		OK,
		MAX
	};

	// ファイルID
	const wchar_t* const FILE_COMMON_FRAME			= L"I_CommonFrame";
	const wchar_t* const FILE_COMMON_SELECT_FRAME	= L"I_CommonSelectFrame";
	const wchar_t* const FILE_SAVE_ARM				= L"I_SaveArm";
	const wchar_t* const FILE_SAVE_ARM_FRAME		= L"I_SaveArmFrame";
	const wchar_t* const FILE_SAVE_WINDOW			= L"I_SaveWindow";
	const wchar_t* const FILE_DECIDE_ARM			= L"I_DecideArm";
	const wchar_t* const FILE_DECIDE_ARM_FRAME		= L"I_DecideArmFrame";
	const wchar_t* const FILE_DECIDE_WINDOW			= L"I_DecideWindow";
	const wchar_t* const FILE_ALERT_WINDOW			= L"I_OptionWindow";
	const wchar_t* const FILE_RANKING_WINDOW		= L"I_RankingWindow";
	const wchar_t* const FILE_RANKING_ARROW			= L"I_RankingArrow";
	const wchar_t* const FILE_RANK_S				= L"I_RankS";
	const wchar_t* const FILE_RANK_A				= L"I_RankA";
	const wchar_t* const FILE_RANK_B				= L"I_RankB";
	const wchar_t* const FILE_RANK_C				= L"I_RankC";
	const wchar_t* const FILE_CANCEL				= L"S_Cancel";
	const wchar_t* const FILE_DECIDE				= L"S_Decide";
	const wchar_t* const FILE_MOVE					= L"S_CursorMove";
	const wchar_t* const FILE_BGM					= L"B_Title";

	/* メニュー */
	// 文字列
	const std::vector<std::wstring> STR_MENU =
	{
		L"スタート",
		L"ランキング",
		L"オプション",
		L"エンド",
	};
	// 描画位置
	constexpr int DRAW_MENU_X = static_cast<int>(Game::WINDOW_W * 0.5f);
	constexpr int DRAW_MENU_Y = 340;
	// 描画間隔
	constexpr int DRAW_MENU_X_INTERVAL_1 = 48;
	constexpr int DRAW_MENU_X_INTERVAL_2 = 256;
	constexpr int DRAW_MENU_Y_INTERVAL = 96;
	// フォントサイズ
	constexpr int FONT_SIZE_MENU = 48;

	/* セーブデータ選択 */
	// 文字列
	const std::vector<std::wstring> STR_SAVE =
	{
		L"セーブデータ1",
		L"セーブデータ2",
		L"セーブデータ3",
	};
	// 描画位置
	constexpr int DRAW_SAVE_X = 755;
	constexpr int DRAW_SAVE_Y = 340;
	// 描画間隔
	constexpr int DRAW_SAVE_X_INTERVAL = 40;
	constexpr int DRAW_SAVE_Y_INTERVAL = 90;
	// フォントサイズ
	constexpr int FONT_SIZE_SAVE = 36;
	// ファイルサイズ
	constexpr float FILE_SIZE_SAVE = 0.75f;
	// ウィンドウ位置
	constexpr int DRAW_SAVE_WINDOW_X = 735;
	constexpr int DRAW_SAVE_WINDOW_Y = 430;
	// アームの位置との差
	constexpr int DRAW_SAVE_SUB_ARM_FRAME_X = 145;
	constexpr int DRAW_SAVE_SUB_ARM_FRAME_Y = 0;
	// アームフレームの位置との差
	constexpr int DRAW_SAVE_SUB_ARM_X = 222;
	constexpr int DRAW_SAVE_SUB_ARM_Y = 90;
	// セーブデータ情報描画位置
	constexpr int DRAW_SAVE_INFO_X = 1000;
	constexpr int DRAW_SAVE_INFO_Y = 300;
	constexpr int DRAW_SAVE_INFO_AND_DECIDE_OUT_VAL = 100;
	// セーブデータ情報描画間隔
	constexpr int DRAW_SAVE_INFO_TIME_Y = 32;
	constexpr int DRAW_SAVE_INFO_CLEAR_NUM_Y = 64;

	/* データ決定 */
	// 文字列
	const std::vector<std::wstring> STR_DECIDE =
	{
		L"続きから",
		L"初めから",
	};
	// 描画位置
	constexpr int DRAW_DECIDE_X = 1130;
	constexpr int DRAW_DECIDE_Y = 312;
	// 描画間隔
	constexpr int DRAW_DECIDE_X_INTERVAL = 48;
	constexpr int DRAW_DECIDE_Y_INTERVAL = 54;
	// フォントサイズ
	constexpr int FONT_SIZE_DECIDE = 32;
	// ファイルサイズ
	constexpr int FONT_SIZE_SAVE_INFO = 24;
	constexpr float FILE_SIZE_DECIDE = 0.5f;
	// ウィンドウ位置
	constexpr int DRAW_DECIDE_WINDOW_X = 1095;
	constexpr int DRAW_DECIDE_WINDOW_Y = DRAW_SAVE_Y;
	// アームの位置との差
	constexpr int DRAW_DECIDE_SUB_ARM_FRAME_X = 115;
	constexpr int DRAW_DECIDE_SUB_ARM_FRAME_Y = 0;
	// アームフレームの位置との差
	constexpr int DRAW_DECIDE_SUB_ARM_X = 185;
	constexpr int DRAW_DECIDE_SUB_ARM_Y = 0;
	// アーム描画間隔
	constexpr int DRAW_DECIDE_WINDOW_INTERVAL_Y = DRAW_SAVE_Y_INTERVAL;

	/* 削除確認 */
	// 文字列
	std::vector<std::wstring> STR_CHECK =
	{
		L"キャンセル",
		L"削除する",
	};
	const std::wstring STR_ALERT = L"削除して始めますか？";
	// 描画位置
	constexpr int DRAW_CHECK_X = 490;
	constexpr int DRAW_CHECK_Y = 450;
	// 描画間隔
	constexpr int DRAW_CHECK_X_INTERVAL = 300;
	// フォントサイズ
	constexpr float FILE_SIZE_CHECK = 0.75f;
	constexpr int FONT_SIZE_CHECK = 32;
	// 画面外まで動かす量
	constexpr int DRAW_ALERT_OUT_VAL = 700;

	/* 警告関係 */
	// 文字カラー
	constexpr unsigned int COLOR_ALERT = 0xffffff;
	// 描画位置
	constexpr int DRAW_ALERT_X = Game::CENTER_X;
	constexpr int DRAW_ALERT_Y = Game::CENTER_Y;
	// 警告文字との差
	constexpr int DRAW_ALERT_STR_SUB_Y = 100;
	// フォントサイズ
	constexpr int FONT_SIZE_ALERT = 48;
	// 揺らす大きさ
	constexpr int SHAKE_SIZE_ALERT = 2;
	// 画像サイズ
	constexpr float FILE_SIZE_ALERT = 0.75f;

	/* ランキング */
	// 矢印描画位置
	constexpr int DRAW_RANKING_ARROW_SUB_X = 500;
	constexpr int DRAW_RANKING_ARROW_SUB_Y = 275;
	// 矢印スワイプの大きさ
	constexpr int DRAW_RANKING_ARROW_X_SWIPE_SIZE = 32;
	// 矢印スワイプスピード
	constexpr float SWIPE_SPEED_ARROW = 1.3f;
	// ステージ名描画
	constexpr unsigned int COLOR_RANKING = 0xffffff;
	// ステージ名描画位置
	constexpr int DRAW_RANKING_STAGE_NAME_SUB_X = 0;
	constexpr int DRAW_RANKING_STAGE_NAME_SUB_Y = 275;
	// ステージ名フォントサイズ
	constexpr int FONT_SIZE_RANKING_STAGE_NAME = 64;
	// ステージ名ウェーブの大きさ
	constexpr int DRAW_RANKING_STAGE_NAME_WAVE_SIZE = 16;
	// ステージ名ウェーブスピード
	constexpr float WAVE_SPEED_STAGE_NAME = 2.0f;
	// 文字描画位置
	constexpr int DRAW_RANKING_STR_SUB_X = 580;
	constexpr int DRAW_RANKING_STR_SUB_Y = 168;
	// 文字描画間隔
	constexpr int DRAW_RANKING_STR_X_INTERVAL = 600;
	constexpr int DRAW_RANKING_STR_Y_INTERVAL = 110;
	// 「記録なし」ずれ具合
	constexpr int DRAW_RANKING_NOT_DATA_SUB_X = 180;
	// 時間ずれ具合
	constexpr int DRAW_RANKING_TIME_SUB_X = 400;
	// ランクずれ具合
	constexpr int DRAW_RANKING_RANK_SUB_X = 160;
	// ランキング文字フォントサイズ
	constexpr int FONT_SIZE_RANKING_RANK = 64;
	constexpr int FONT_SIZE_RANKING_TIME = 56;
	// ランク画像サイズ
	constexpr float FILE_SIZE_RANKING_RANK = 0.125f;
	constexpr float TIME_SIZE = 0.5f;
	// ランキングタイムカラー
	constexpr unsigned int COLOR_TIME = 0xffffff;
	// ランク毎のファイルパス
	const std::unordered_map<RankKind, int> RANK_FILE_PATH =
	{
		{ RankKind::S, I_RANK_S },
		{ RankKind::A, I_RANK_A },
		{ RankKind::B, I_RANK_B },
		{ RankKind::C, I_RANK_C },
	};

	/* PadUi描画 */
	constexpr int DRAW_PAD_X = 1100;		// 基準Y描画位置
	constexpr int DRAW_PAD_Y = 680;			// 画像描画位置
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int DRAW_PAD_FRAME_X = 1160;			// フレーム描画位置
	constexpr float FILE_SIZE_PAD_FRAME = 0.525f;	// フレームサイズ
	constexpr int DRAW_PAD_STR_X = 1120;	// 文字列描画位置
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int PAD_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー
	// 描画間隔
	constexpr int DRAW_PAD_X_INTERVAL = 20;	
	constexpr int DRAW_PAD_Y_INTERVAL = 50;	
	/* PadUI(ランキングのみ) */
	constexpr int DRAW_PAD_Y_RANKING = 690;		// 基準Y描画位置
	constexpr int DRAW_PAD_X_RANKING = 1130;	// 画像描画位置
	constexpr int DRAW_PAD_FRAME_X_RANKING = 1190;	// フレーム描画位置
	constexpr int DRAW_PAD_STR_X_RANKING = 1150;	// 文字列描画位置
	constexpr int DRAW_PAD_ALPHA = static_cast<int>(Game::ALPHA_VAL * 0.75f);

	/* その他共通とかの */
	// 選択文字カラー
	constexpr unsigned int COLOR_SELECT = 0x000000;
	// 未選択文字カラー
	constexpr unsigned int COLOR_NOT_SELECT = 0xffffff;
	// 文字ウェーブの大きさ
	constexpr int STR_WAVE_SIZE = 16;
	// ウェーブスピード
	constexpr float STR_WAVE_SPEED = 2;
	// 画面外に飛ばす用の値
	constexpr int DRAW_OUT_VAL = 900;
	// フェードにかかるフレーム
	constexpr int FADE_FRAME = 20;
	// UIの移動速度
	constexpr float MOVE_SPEED = 1.0f / FADE_FRAME;
	// 時間のフェード速度
	constexpr int TIME_FADE_SPEED = 2;

	// ステージ名
	const wchar_t* const STAGE_NAME = L"Title";
	// カメラの位置
	const Vec3 CAMERA_POS = Vec3(-36.0f, 7.5f, 24.0f);
	// カメラの向き
	const Vec3 CAMERA_DIR = Vec3(-1.0f, 0.0f, 1.0f);
	// ゲート弾を打つまでのフレーム
	constexpr int WAIT_SHOT_FRAME = 360;
	// 弾の発射位置
	const Vec3 SHOT_POS = Vec3(-24.0f, 7.5f, 24.0f);
	// 弾の発射方向
	const Vec3 SHOT_DIR = Vec3(-0.8f, 0.0f, 1.0f);
	// RT
	enum class RTKind
	{
		GATE_A_TEX,
		GATE_B_TEX,
		TEMP_1,
		TEMP_2,
		MAX,
	};
}

SceneTitle::SceneTitle() :
	SceneBase(SceneKind::TITLE),
	m_updateFunc(&SceneTitle::SelectMenuUpdate),
	m_drawFunc(&SceneTitle::DrawSelectMenu),
	m_state(State::MENU),
	m_waitShotFrame(0),
	m_menuCurrent(0),
	m_saveDataNo(0),
	m_preSaveDataNo(0),
	m_decideCurrent(0),
	m_checkCurrent(0),
	m_rankingCurrent(0),
	m_fadeFrame(0),
	m_waveCount(0),
	m_isShot(false),
	m_isFade(false),
	m_isFadeTime(false),
	m_isDrawMenu(true),
	m_isDrawSelectSave(false)
{
}

void SceneTitle::AsyncInit()
{
	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[I_COMMON_FRAME]			= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_COMMON_SELECT_FRAME]	= fileMgr.Load(I_COMMON_SELECT_FRAME);
	m_files[I_SAVE_ARM]				= fileMgr.Load(I_SAVE_ARM);
	m_files[I_SAVE_ARM_FRAME]		= fileMgr.Load(I_SAVE_ARM_FRAME);
	m_files[I_SAVE_WINDOW]			= fileMgr.Load(I_SAVE_WINDOW);
	m_files[I_DECIDE_ARM]			= fileMgr.Load(I_DECIDE_ARM);
	m_files[I_DECIDE_ARM_FRAME]		= fileMgr.Load(I_DECIDE_ARM_FRAME);
	m_files[I_DECIDE_WINDOW]		= fileMgr.Load(I_DECIDE_WINDOW);
	m_files[I_OPTION_WINDOW]		= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_RANKING_WINDOW]		= fileMgr.Load(I_RANKING_WINDOW);
	m_files[I_RANKING_ARROW]		= fileMgr.Load(I_RANKING_ARROW);
	m_files[I_RANK_S]				= fileMgr.Load(I_RANK_S);
	m_files[I_RANK_A]				= fileMgr.Load(I_RANK_A);
	m_files[I_RANK_B]				= fileMgr.Load(I_RANK_B);
	m_files[I_RANK_C]				= fileMgr.Load(I_RANK_C);
	m_files[I_PAD_A]				= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]				= fileMgr.Load(I_PAD_B);
	m_files[S_CANCEL]				= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]				= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]			= fileMgr.Load(S_CURSOR_MOVE);
	m_files[S_SHOT_PLAYER]			= fileMgr.Load(S_SHOT_PLAYER);
	m_files[B_TITLE]				= fileMgr.Load(B_TITLE);
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.LoadFile(E_GATE_BULLET_BLUE);
	effMgr.LoadFile(E_GATE_BULLET_ORANGE);
	effMgr.LoadFile(E_LASER_BULLET);

	// マネージャー生成
	m_stageMgr = std::make_shared<StageManager>(STAGE_NAME);
	m_cameraMgr = std::make_shared<CameraManager>();
	m_gateMgr = std::make_shared<GateManager>(m_cameraMgr, STAGE_NAME);
	m_player = std::make_shared<Player>(std::dynamic_pointer_cast<PlayerCamera>(m_cameraMgr->GetCamera(CameraKind::PLAYER)), m_gateMgr);
	// 初期化(非同期)
	m_stageMgr->AsyncInit();
}

void SceneTitle::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_TITLE)->GetHandle());
	// 画面生成
	m_rtTable.resize(static_cast<int>(RTKind::MAX));
	for (auto& handle : m_rtTable) handle = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
	// UIデータ生成
	m_menuUIList		= UIMoveData::Make(static_cast<int>(MenuCurrent::MAX), DRAW_MENU_X, DRAW_MENU_Y, 0, DRAW_MENU_Y_INTERVAL);
	m_saveUIList		= UIMoveData::Make(SaveDataManager::GetInstance().GetSaveDataNum(), DRAW_SAVE_X + DRAW_OUT_VAL, DRAW_SAVE_Y, 0, DRAW_SAVE_Y_INTERVAL);
	m_decideUIList		= UIMoveData::Make(static_cast<int>(DecideCurrent::MAX), DRAW_DECIDE_X + DRAW_OUT_VAL, DRAW_DECIDE_Y, 0, DRAW_DECIDE_Y_INTERVAL);
	m_checkUIList		= UIMoveData::Make(static_cast<int>(CheckCurrent::MAX), DRAW_CHECK_X, DRAW_CHECK_Y - DRAW_ALERT_OUT_VAL, DRAW_CHECK_X_INTERVAL, 0);
	m_saveInfoUI		= UIMoveData::Make(DRAW_SAVE_INFO_X + DRAW_OUT_VAL, DRAW_SAVE_INFO_Y);
	m_saveArmWindowUI	= UIMoveData::Make(DRAW_SAVE_WINDOW_X + DRAW_OUT_VAL, DRAW_SAVE_WINDOW_Y);
	m_decideArmWindowUI = UIMoveData::Make(DRAW_DECIDE_WINDOW_X + DRAW_OUT_VAL, DRAW_DECIDE_WINDOW_Y);
	m_alertUI			= UIMoveData::Make(DRAW_ALERT_X, DRAW_ALERT_Y - DRAW_ALERT_OUT_VAL);
	m_rankingUI			= UIMoveData::Make(Game::CENTER_X, Game::CENTER_Y);
	// 初期化
	m_menuUIList[0]->ChangeHorizontal(DRAW_MENU_X - DRAW_MENU_X_INTERVAL_1);
	m_stageMgr->Init(m_player.get(), m_gateMgr.get());
	m_gateMgr->Init(m_player);
	m_player->Init(CAMERA_POS, CAMERA_DIR);
}

void SceneTitle::End()
{
	m_stageMgr->End();
	m_cameraMgr->End();
	m_gateMgr->End();

	for (auto& handle : m_rtTable) DeleteGraph(handle);
	EffekseerManager::GetInstance().ReleaseFile();
}

void SceneTitle::Update(bool isFade)
{
	// ウェーブ更新
	++m_waveCount;
	// UI更新
	m_alertUI->Update(MOVE_SPEED);
	m_saveArmWindowUI->Update(MOVE_SPEED);
	m_decideArmWindowUI->Update(MOVE_SPEED, EasingType::LERP);
	m_saveInfoUI->Update(MOVE_SPEED, EasingType::LERP);
	for (auto& data : m_menuUIList)	data->Update(MOVE_SPEED);
	for (auto& data : m_saveUIList)	data->Update(MOVE_SPEED);
	for (auto& data : m_decideUIList) data->Update(MOVE_SPEED);
	for (auto& data : m_checkUIList) data->Update(MOVE_SPEED);
	// 入力による動作更新
	StageUpdate();

	if (isFade) return;
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::TITLE);
		return;
	}
	(this->*m_updateFunc)();
	if (m_isFade)
	{
		--m_fadeFrame;
		if (m_fadeFrame < 0)
		{
			m_fadeFrame = 0;
			m_isFade = false;
		}
	}
	if (m_isFadeTime)
	{
		m_fadeFrame -= TIME_FADE_SPEED;
		if (m_fadeFrame < -FADE_FRAME)
		{
			m_fadeFrame = 0;
			m_isFadeTime = false;
		}
	}
}

void SceneTitle::Draw() const
{
	auto& fontMgr = FontManager::GetInstance();

	/* 共通で描画する部分 */
	DrawStage();
	// 画像ハンドル
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	// メインとなる部分描画
	if (m_isDrawMenu)	UIUtility::DrawFrameAndStr(m_menuUIList, 1.0f, 0.0f, FONT_SIZE_MENU, m_menuCurrent, STR_MENU, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, STR_WAVE_SIZE, m_waveCount * STR_WAVE_SPEED, true);
	else				UIUtility::DrawFrameAndStr(m_menuUIList, 1.0f, 0.0f, FONT_SIZE_MENU, m_menuCurrent, STR_MENU, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
	// 追加情報部分の枠描画
	ArmWindowDraw(m_saveArmWindowUI, DRAW_SAVE_SUB_ARM_FRAME_X, DRAW_SAVE_SUB_ARM_FRAME_Y, DRAW_SAVE_SUB_ARM_X, DRAW_SAVE_SUB_ARM_Y, I_SAVE_WINDOW, I_SAVE_ARM_FRAME, I_SAVE_ARM);
	ArmWindowDraw(m_decideArmWindowUI, DRAW_DECIDE_SUB_ARM_FRAME_X, DRAW_DECIDE_SUB_ARM_FRAME_Y, DRAW_DECIDE_SUB_ARM_X, DRAW_DECIDE_SUB_ARM_Y, I_DECIDE_WINDOW, I_DECIDE_ARM_FRAME, I_DECIDE_ARM);
	// セーブデータ選択のUI描画
	if (m_isDrawSelectSave) UIUtility::DrawFrameAndStr(m_saveUIList, FILE_SIZE_SAVE, 0.0f, FONT_SIZE_SAVE, m_saveDataNo, STR_SAVE, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, STR_WAVE_SIZE, m_waveCount * STR_WAVE_SPEED, true);
	else					UIUtility::DrawFrameAndStr(m_saveUIList, FILE_SIZE_SAVE, 0.0f, FONT_SIZE_SAVE, m_saveDataNo, STR_SAVE, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
	// それぞれでの描画処理
	(this->*m_drawFunc)();
	// PadUI
	DrawPadUI();
}

void SceneTitle::StageUpdate()
{
	// ゲート弾を打っていない場合
	if (!m_isShot)
	{
		// 待機時間を経過したら発射
		++m_waitShotFrame;
		if (m_waitShotFrame > WAIT_SHOT_FRAME)
		{
			SoundManager::GetInstance().PlaySe(m_files.at(S_SHOT_PLAYER)->GetHandle());
			// 生成
			auto bullet = std::make_shared<GateBullet>(m_gateMgr, GateKind::Orange);
			// カメラの向いている方向に弾を進ませる
			bullet->Init(SHOT_POS, SHOT_DIR);
			// 追加
			m_gateMgr->AddBullet(bullet);

			m_isShot = true;
		}
	}

	m_stageMgr->Update();
	m_gateMgr->Update();
}

void SceneTitle::SelectMenuUpdate()
{
	if (CurrentUpdate(m_menuCurrent, m_menuUIList, DRAW_MENU_X, DRAW_MENU_X_INTERVAL_1, Command::BTN_UP, Command::BTN_DOWN))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		m_waveCount = 0;
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_DECIDE)->GetHandle());
		// セーブデータ選択に移動
		if (m_menuCurrent == static_cast<int>(MenuCurrent::SELECT_SAVE))
		{
			OnSelectSaveData();
		}
		// ランキングを開く
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::RANKING))
		{
			OnRanking();
		}
		// オプションを開く
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::OPOTION))
		{
			m_scnMgr.OpenOption(SceneKind::TITLE);
		}
		// ゲーム終了
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::END))
		{
			auto& app = Application::GetInstance();
			app.OnEnd();
		}
	}
}

void SceneTitle::RankingUpdate()
{
	bool isMove = false;
	auto stageNum = StageDataManager::GetInstance().GetStageNum() - 2;
	if (CursorUtility::CursorUp(m_rankingCurrent, stageNum, Command::BTN_LEFT)) isMove = true;
	if (CursorUtility::CursorDown(m_rankingCurrent, stageNum, Command::BTN_RIGHT)) isMove = true;
	if (isMove)
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
	}

	auto& input = Input::GetInstance();
	// キャンセル
	if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		OnSelectMenu();
	}
}

void SceneTitle::SelectSaveDataUpdate()
{
	// フェードしていない場合またはフェードフレームが-の場合は現在の番号を保存
	if (!m_isFadeTime || m_fadeFrame < 0) m_preSaveDataNo = m_saveDataNo;
	// カーソルが移動したら
	if (CurrentUpdate(m_saveDataNo, m_saveUIList, DRAW_SAVE_X, DRAW_SAVE_X_INTERVAL, Command::BTN_UP, Command::BTN_DOWN))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		// ウェーブカウント初期化
		m_waveCount = 0;
		// 決定側のwindowとセーブデータ情報の位置を移動
		m_decideArmWindowUI->ChangeVertical(DRAW_DECIDE_WINDOW_Y + DRAW_DECIDE_WINDOW_INTERVAL_Y * m_saveDataNo, -1, true);
		m_saveInfoUI->ChangeVertical(DRAW_SAVE_INFO_Y + DRAW_DECIDE_WINDOW_INTERVAL_Y * m_saveDataNo, -1, true);
		// フレーム変更
		m_fadeFrame = FADE_FRAME;
		// フェードしている状態に
		m_isFadeTime = true;
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_DECIDE)->GetHandle());
		auto& saveDataMgr = SaveDataManager::GetInstance();
		// セーブデータが存在する場合
		if (saveDataMgr.IsExist(m_saveDataNo))
		{
			// セーブデータ決定に移動
			OnDecideSaveData();
			m_decideCurrent = 0;
			m_fadeFrame = FADE_FRAME;
			m_isFade = true;
		}
		// 存在しない場合
		else
		{
			// ゲームスタート
			OnStart();
		}
		
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		// メニュー選択に戻る
		OnSelectMenu();
	}
}

void SceneTitle::DecideSaveDataUpdate()
{
	if (CurrentUpdate(m_decideCurrent, m_decideUIList, DRAW_DECIDE_X, DRAW_DECIDE_X_INTERVAL, Command::BTN_UP, Command::BTN_DOWN))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		m_waveCount = 0;
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_DECIDE)->GetHandle());
		// 続きから
		if (m_decideCurrent == static_cast<int>(DecideCurrent::CONTINU))
		{
			// ゲームスタート
			OnStart();
		}
		// 初めから
		else if (m_decideCurrent == static_cast<int>(DecideCurrent::DELETE_DATA))
		{
			// 削除確認に移動
			OnCheckDeleteSaveData();
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		// セーブデータ選択に移動
		OnSelectSaveData();
		m_isFade = true;
		m_fadeFrame = FADE_FRAME;
	}
}

void SceneTitle::CheckDeleteSaveDataUpdate()
{
	bool isMove = false;
	if (CursorUtility::CursorUp<CheckCurrent>(m_checkCurrent, CheckCurrent::MAX, Command::BTN_LEFT)) isMove = true;
	if (CursorUtility::CursorDown<CheckCurrent>(m_checkCurrent, CheckCurrent::MAX, Command::BTN_RIGHT)) isMove = true;
	if (isMove)
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		m_waveCount = 0;
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_DECIDE)->GetHandle());
		// キャンセル
		if (m_checkCurrent == static_cast<int>(CheckCurrent::CANCEL))
		{
			// セーブデータ決定に移動
			OnDecideSaveData();
		}
		// 削除
		else if (m_checkCurrent == static_cast<int>(CheckCurrent::OK))
		{
			// 初期化
			auto& saveDataMgr = SaveDataManager::GetInstance();
			saveDataMgr.Initialized(m_saveDataNo);
			// ゲームスタート
			OnStart();
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		// セーブデータ決定に移動
		OnDecideSaveData();
	}
}

void SceneTitle::OnSelectMenu()
{
	m_state = State::MENU;
	// メニューに分類されているものを左から中央に戻す
	UIMoveData::ChangeHorizontal(m_menuUIList, DRAW_MENU_X, m_menuCurrent, DRAW_MENU_X - DRAW_MENU_X_INTERVAL_1);
	// Window関係のものを画面内から画面外に移す
	UIMoveData::ChangeHorizontal(m_saveUIList, DRAW_SAVE_X + DRAW_OUT_VAL);
	m_saveArmWindowUI->ChangeHorizontal(DRAW_SAVE_WINDOW_X + DRAW_OUT_VAL);
	m_decideArmWindowUI->ChangeHorizontal(DRAW_DECIDE_WINDOW_X + DRAW_OUT_VAL);
	m_saveInfoUI->ChangeHorizontal(DRAW_SAVE_INFO_X + DRAW_OUT_VAL);
	// メンバ関数変更
	m_updateFunc = &SceneTitle::SelectMenuUpdate;
	m_drawFunc = &SceneTitle::DrawSelectMenu;
	// カウント初期化
	m_waveCount = 0;
	m_isDrawMenu = true;
	m_isDrawSelectSave = false;
}

void SceneTitle::OnRanking()
{
	m_state = State::RANKING;
	// メンバ関数変更
	m_updateFunc = &SceneTitle::RankingUpdate;
	m_drawFunc = &SceneTitle::DrawRanking;
}

void SceneTitle::OnSelectSaveData()
{
	m_state = State::SELECT_SAVE_DATA;
	// メニューに分類されているものを中央から左にずらす
	UIMoveData::ChangeHorizontal(m_menuUIList, DRAW_MENU_X - DRAW_MENU_X_INTERVAL_2, m_menuCurrent, DRAW_MENU_X - DRAW_MENU_X_INTERVAL_1 - DRAW_MENU_X_INTERVAL_2);
	// 画面外にあるWindowたちを画面内に持ってくる
	UIMoveData::ChangeHorizontal(m_saveUIList, DRAW_SAVE_X, m_saveDataNo, DRAW_SAVE_X - DRAW_SAVE_X_INTERVAL);
	m_decideArmWindowUI->ChangeHorizontal(DRAW_DECIDE_WINDOW_X);
	m_saveArmWindowUI->ChangeHorizontal(DRAW_SAVE_WINDOW_X);
	m_saveInfoUI->ChangeHorizontal(DRAW_SAVE_INFO_X);
	UIMoveData::ChangeHorizontal(m_decideUIList, DRAW_DECIDE_X + DRAW_SAVE_INFO_AND_DECIDE_OUT_VAL, m_decideCurrent, DRAW_DECIDE_X + DRAW_SAVE_INFO_AND_DECIDE_OUT_VAL - DRAW_DECIDE_X_INTERVAL);
	// メンバ関数変更
	m_updateFunc = &SceneTitle::SelectSaveDataUpdate;
	m_drawFunc = &SceneTitle::DrawSelectSaveData;
	// カウント初期化
	m_waveCount = 0;
	m_isDrawMenu = false;
	m_isDrawSelectSave = true;
}

void SceneTitle::OnDecideSaveData()
{
	UIMoveData::ChangeVertical(m_checkUIList, DRAW_CHECK_Y - DRAW_ALERT_OUT_VAL);
	m_alertUI->ChangeVertical(DRAW_ALERT_Y - DRAW_ALERT_OUT_VAL);
	m_saveInfoUI->ChangeHorizontal(DRAW_SAVE_INFO_X + DRAW_SAVE_INFO_AND_DECIDE_OUT_VAL);
	UIMoveData::ChangeHorizontal(m_decideUIList, DRAW_DECIDE_X, m_decideCurrent, DRAW_DECIDE_X - DRAW_DECIDE_X_INTERVAL);
	m_state = State::DECIDE_SAVE_DATA;
	// メンバ関数変更
	m_updateFunc = &SceneTitle::DecideSaveDataUpdate;
	m_drawFunc = &SceneTitle::DrawDecideSaveData;
	// カウント初期化
	m_waveCount = 0;
	m_isDrawSelectSave = false;
}

void SceneTitle::OnCheckDeleteSaveData()
{
	UIMoveData::ChangeVertical(m_checkUIList, DRAW_CHECK_Y);
	m_alertUI->ChangeVertical(DRAW_ALERT_Y);
	m_state = State::CHECK_DELETE;
	// メンバ関数変更
	m_updateFunc = &SceneTitle::CheckDeleteSaveDataUpdate;
	m_drawFunc = &SceneTitle::DrawCheckDeleteSaveData;
	// カウント初期化
	m_waveCount = 0;
	// チェック位置初期化
	m_checkCurrent = 0;
}

void SceneTitle::DrawStage() const
{
	// ゲートの生成状況に合わせて変更
	if (m_gateMgr->IsCreateBothGates()) DrawBlend();
	else								DrawNormal();
}

void SceneTitle::DrawNormal() const
{
	DrawModelBlend(DX_SCREEN_BACK, -1, -1, CameraKind::PLAYER, m_cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps());
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.SyncCamera();
	effMgr.Draw();
}

void SceneTitle::DrawBlend() const
{
	auto rtGateA = m_rtTable[static_cast<int>(RTKind::GATE_A_TEX)];
	auto rtGateB = m_rtTable[static_cast<int>(RTKind::GATE_B_TEX)];

	// それぞれのゲートから見た景色を描画
	DrawGateBlend(rtGateA, CameraKind::GATE_A, CameraKind::GATE_A_FROM_B);
	DrawGateBlend(rtGateB, CameraKind::GATE_B, CameraKind::GATE_B_FROM_A);

	DrawModelBlend(DX_SCREEN_BACK, rtGateA, rtGateB, CameraKind::PLAYER, m_cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps());
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.SyncCamera();
	effMgr.Draw();
}

void SceneTitle::DrawGateBlend(int rt, CameraKind gate, CameraKind gateFrom) const
{
	auto rtTemp1 = m_rtTable[static_cast<int>(RTKind::TEMP_1)];
	auto rtTemp2 = m_rtTable[static_cast<int>(RTKind::TEMP_2)];

	{
		SetDrawScreen(rtTemp1);
		ClearDrawScreen();
		m_cameraMgr->AppInfo(gateFrom);

		m_stageMgr->Draw();
	}
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

		// オブジェクト描画
		m_stageMgr->Draw();
	}
	// ゲートからの画面を描画してあげる
	DrawModelBlend(rt, rtTemp2, rtTemp2, gate, true);
}

void SceneTitle::DrawModelBlend(int rt, int tex1, int tex2, CameraKind camera, bool isPlayerDraw) const
{
	// 使用RTの決定
	SetDrawScreen(rt);
	ClearDrawScreen();
	// カメラの適用
	m_cameraMgr->AppInfo(camera);
	// ゲートの描画
	m_gateMgr->DrawGate(tex1, tex2);
	// ステージの描画
	m_stageMgr->Draw();
}

void SceneTitle::DrawSelectMenu() const
{
	DrawSaveInfo(m_saveDataNo);
}

void SceneTitle::DrawRanking() const
{
	auto& fontMgr = FontManager::GetInstance();
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageName = stageDataMgr.GetStageName(m_rankingCurrent + 1);
	auto& rnkMgr = RankingDataManager::GetInstance();
	// ウィンドウ描画
	DrawRotaGraphFast(m_rankingUI->x, m_rankingUI->y, 1.0f, 0.0f, m_files.at(I_RANKING_WINDOW)->GetHandle(), true);
	// 矢印描画
	int addArrowX = static_cast<int>(std::sin(m_waveCount * Game::DEG_2_RAD * SWIPE_SPEED_ARROW) * DRAW_RANKING_ARROW_X_SWIPE_SIZE);
	DrawRotaGraphFast(m_rankingUI->x - DRAW_RANKING_ARROW_SUB_X + addArrowX, m_rankingUI->y - DRAW_RANKING_ARROW_SUB_Y, 1.0f, 0.0f, m_files.at(I_RANKING_ARROW)->GetHandle(), true);
	DrawRotaGraphFast(m_rankingUI->x + DRAW_RANKING_ARROW_SUB_X - addArrowX, m_rankingUI->y - DRAW_RANKING_ARROW_SUB_Y, 1.0f, 180.0f * Game::DEG_2_RAD, m_files.at(I_RANKING_ARROW)->GetHandle(), true);
	UIUtility::DrawWaveStr(m_rankingUI->x - DRAW_RANKING_STAGE_NAME_SUB_X, m_rankingUI->y - DRAW_RANKING_STAGE_NAME_SUB_Y, COLOR_RANKING, stageName, FONT_SIZE_RANKING_STAGE_NAME, m_waveCount * WAVE_SPEED_STAGE_NAME, DRAW_RANKING_STAGE_NAME_WAVE_SIZE, true);

	// 順位、タイム描画
	constexpr int halfNum = static_cast<int>(RankingDataManager::RANKING_DATA_NUM * 0.5f);
	int drawStrBaseX = m_rankingUI->x - DRAW_RANKING_STR_SUB_X;
	int drawStrBaseY = m_rankingUI->y - DRAW_RANKING_STR_SUB_Y;
	for (int i = 0; i < RankingDataManager::RANKING_DATA_NUM; ++i)
	{
		int x = drawStrBaseX + DRAW_RANKING_STR_X_INTERVAL * (i / halfNum);
		int y = drawStrBaseY + DRAW_RANKING_STR_Y_INTERVAL * (i % halfNum);
		std::wostringstream oss;
		oss << i + 1 << L"位";
		UIUtility::DrawShadowStrLeft(x, y, 2, 2, COLOR_RANKING, oss.str(), FONT_SIZE_RANKING_RANK);
		auto timeFrame = rnkMgr.GetTime(stageName, i);
		// 記録がない場合
		if (timeFrame < 0)
		{
			UIUtility::DrawShadowStrLeft(x + DRAW_RANKING_NOT_DATA_SUB_X, y, 2, 2, COLOR_RANKING, L"記録がありません", FONT_SIZE_RANKING_TIME);
		}
		// 記録がある場合
		else
		{
			const auto& rank = stageDataMgr.GetRank(stageName, timeFrame);
			const auto& path = RANK_FILE_PATH.at(rank);
			DrawRotaGraphFast(x + DRAW_RANKING_RANK_SUB_X, y, FILE_SIZE_RANKING_RANK, 0.0f, m_files.at(path)->GetHandle(), true);
			TimeUtility::DrawTime(x + DRAW_RANKING_TIME_SUB_X, y, timeFrame, TIME_SIZE, COLOR_TIME, true);
		}
	}
}

void SceneTitle::DrawSelectSaveData() const
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	auto& stageDataMgr = StageDataManager::GetInstance();
	if (m_isFade)
	{
		int alpha = static_cast<int>(Game::ALPHA_VAL * (m_fadeFrame / static_cast<float>(FADE_FRAME)));
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// 画像ハンドル
		int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
		int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
		// 続きから・初めからのUI描画
		UIUtility::DrawFrameAndStr(m_decideUIList, FILE_SIZE_DECIDE, 0.0f, FONT_SIZE_DECIDE, m_decideCurrent, STR_DECIDE, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Game::ALPHA_VAL - alpha);
	}
	else if (m_isFadeTime)
	{
		int alpha = static_cast<int>(Game::ALPHA_VAL * (m_fadeFrame / static_cast<float>(FADE_FRAME)));
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::max<int>(alpha, 0));
		// セーブデータの情報描画
		DrawSaveInfo(m_preSaveDataNo);
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::max<int>(alpha * -1, 0));
	}
	// セーブデータの情報描画
	DrawSaveInfo(m_saveDataNo);
	// ブレンドなしに
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::DrawDecideSaveData() const
{
	// 画像ハンドル
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	if (m_isFade)
	{
		int alpha = static_cast<int>(Game::ALPHA_VAL * (m_fadeFrame / static_cast<float>(FADE_FRAME)));
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// セーブデータの情報描画
		DrawSaveInfo(m_saveDataNo);
		// 透明度変更
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Game::ALPHA_VAL - alpha);
	}
	// 続きから・初めからのUI描画
	UIUtility::DrawFrameAndStr(m_decideUIList, FILE_SIZE_DECIDE, 0.0f, FONT_SIZE_DECIDE, m_decideCurrent, STR_DECIDE, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, STR_WAVE_SIZE, m_waveCount * STR_WAVE_SPEED, true);
	// ブレンドなしに
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// 警告ウィンドウ描画
	DrawRotaGraphFast(m_alertUI->x, m_alertUI->y, FILE_SIZE_ALERT, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	// 削除確認のUI描画
	UIUtility::DrawFrameAndStr(m_checkUIList, FILE_SIZE_CHECK, 0.0f, FONT_SIZE_CHECK, m_checkCurrent, STR_CHECK, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
}

void SceneTitle::DrawCheckDeleteSaveData() const
{
	// 画像ハンドル
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	// 続きから・初めからのUI描画
	UIUtility::DrawFrameAndStr(m_decideUIList, FILE_SIZE_DECIDE, 0.0f, FONT_SIZE_DECIDE, m_decideCurrent, STR_DECIDE, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
	// 警告ウィンドウ描画
	DrawRotaGraphFast(m_alertUI->x, m_alertUI->y, FILE_SIZE_ALERT, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	// 削除確認のUI描画
	UIUtility::DrawFrameAndStr(m_checkUIList, FILE_SIZE_CHECK, 0.0f, FONT_SIZE_CHECK, m_checkCurrent, STR_CHECK, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::NORMAL, 0, -1.0f, true);
	// 文字描画
	if (m_checkCurrent == static_cast<int>(CheckCurrent::CANCEL))	UIUtility::DrawWaveStr(m_alertUI->x, m_alertUI->y - DRAW_ALERT_STR_SUB_Y, COLOR_ALERT, STR_ALERT, FONT_SIZE_ALERT, m_waveCount * STR_WAVE_SPEED, STR_WAVE_SIZE, true);
	else															UIUtility::DrawShakeStr(m_alertUI->x, m_alertUI->y - DRAW_ALERT_STR_SUB_Y, COLOR_ALERT, STR_ALERT, FONT_SIZE_ALERT, SHAKE_SIZE_ALERT, true);
	
}

void SceneTitle::DrawSaveInfo(int saveNo) const
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	const auto fontH = FontManager::GetInstance().GetHandle(FONT_SIZE_SAVE_INFO);
	if (saveDataMgr.IsExist(saveNo))
	{
		auto& stageDataMgr = StageDataManager::GetInstance();
		const auto time = saveDataMgr.GetPlayTime(saveNo);
		const auto clearNo = saveDataMgr.GetClearStageNum(saveNo);
		const auto stageNum = stageDataMgr.GetStageNum() - 1;
		UIUtility::DrawShadowStrLeft(m_saveInfoUI->x, m_saveInfoUI->y, 2, 2, COLOR_NOT_SELECT, L"ユーザー名：", FONT_SIZE_SAVE_INFO);
		TimeUtility::DrawTime(m_saveInfoUI->x, m_saveInfoUI->y + DRAW_SAVE_INFO_TIME_Y, COLOR_NOT_SELECT, FONT_SIZE_SAVE_INFO, time, L"プレイ時間：", nullptr, true);
		std::wostringstream oss;
		oss << L"クリア数　：" << clearNo << L" / " << stageNum;
		UIUtility::DrawShadowStrLeft(m_saveInfoUI->x, m_saveInfoUI->y + DRAW_SAVE_INFO_CLEAR_NUM_Y, 2, 2, COLOR_NOT_SELECT, oss.str(), FONT_SIZE_SAVE_INFO);
	}
	else
	{
		UIUtility::DrawShadowStrLeft(m_saveInfoUI->x, m_saveInfoUI->y + DRAW_SAVE_INFO_TIME_Y, 2, 2, COLOR_NOT_SELECT, L"データが存在しません", FONT_SIZE_SAVE_INFO);
	}
}

void SceneTitle::DrawPadUI() const
{
	// オプションなら描画しない
	if (m_scnMgr.IsOpenOption()) return;

	const auto fontPadH = FontManager::GetInstance().GetHandle(FONT_SIZE_PAD);
	if (m_state == State::RANKING)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, DRAW_PAD_ALPHA);
		// B:キャンセル
		DrawRotaGraphFast(DRAW_PAD_FRAME_X_RANKING, DRAW_PAD_Y_RANKING, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_PAD_X_RANKING, DRAW_PAD_Y_RANKING, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X_RANKING, DRAW_PAD_Y_RANKING, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"キャンセル", FONT_SIZE_PAD);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		int y = DRAW_PAD_Y;
		// A:決定
		DrawRotaGraphFast(DRAW_PAD_FRAME_X, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
		if (m_state != State::MENU)
		{
			// B:キャンセル
			y -= DRAW_PAD_Y_INTERVAL;
			DrawRotaGraphFast(DRAW_PAD_FRAME_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
			DrawRotaGraphFast(DRAW_PAD_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
			UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X + DRAW_PAD_X_INTERVAL, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"キャンセル", FONT_SIZE_PAD);
		}
	}
}

bool SceneTitle::CurrentUpdate(int& current, const std::vector<std::shared_ptr<UIMoveData>>& list, int start, int interval, const char* const upCmd, const char* const downCmd)
{
	bool isMove = false;
	int pre = current;
	if (CursorUtility::CursorUp(current, list.size(), upCmd)) isMove = true;
	if (CursorUtility::CursorDown(current, list.size(), downCmd)) isMove = true;
	if (isMove)
	{
		list[pre]->ChangeHorizontal(start);
		list[current]->ChangeHorizontal(start - interval);
	}

	return isMove;
}

void SceneTitle::OnStart()
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	// 使用セーブデータの決定
	saveDataMgr.DecideUseSaveNo(m_saveDataNo);
	// シーンの変更
	auto next = std::make_shared<SceneStageSelect>();
	m_scnMgr.Change(next);
}

void SceneTitle::ArmWindowDraw(const std::shared_ptr<UIMoveData>& data, int subArmFrameX, int subArmFrameY, int subArmX, int subArmY, int windowFileName, int armFrameFileName, int armFileName) const
{
	// ウィンドウ
	DrawRotaGraphFast(data->x, data->y, 1.25f, 0.0f, m_files.at(windowFileName)->GetHandle(), true);
	// アームフレーム
	DrawRotaGraphFast(data->x - subArmFrameX, data->y - subArmFrameY, 1.25f, 0.0f, m_files.at(armFrameFileName)->GetHandle(), true);
	// アーム
	DrawRotaGraphFast(data->x - subArmX, data->y - subArmY, 1.0f, 0.0f, m_files.at(armFileName)->GetHandle(), true);
}