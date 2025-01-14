#include "SceneStageSelect.h"
#include <DxLib.h>
#include <sstream>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "SaveDataManager.h"
#include "StageDataManager.h"
#include "SoundManager.h"
#include "Game.h"
#include "Files.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneMain.h"

namespace
{
	enum class CheckCurrent : int
	{
		CANCEL,
		RETURN,
		MAX
	};

	// ファイルID
	const wchar_t* const FILE_WINDOW_BACK		= L"I_SelectWindowBack";
	const wchar_t* const FILE_WINDOW			= L"I_SelectWindow";
	const wchar_t* const FILE_RELEASE_FRAME		= L"I_ReleaseFrame";
	const wchar_t* const FILE_NOT_RELEASE_FRAME = L"I_NotReleaseFrame";
	const wchar_t* const FILE_SELECT_FRAME		= L"I_StageSelectFrame";
	const wchar_t* const FILE_BEST_TIME_FRAME	= L"I_BestTimeFrame";
	const wchar_t* const FILE_DECORATIVE_FRAME	= L"I_DecorativeFrame";
	const wchar_t* const FILE_DIVIDING_LINE		= L"I_DividingLine";
	const wchar_t* const FILE_SELECT_OBI		= L"I_SelectObi";
	const wchar_t* const FILE_STAGE_IMAGE_BASE	= L"I_StageImage";
	const wchar_t* const FILE_COMMON_FRAME		= L"I_CommonFrame";
	const wchar_t* const FILE_PAD_B				= L"I_PadB";
	const wchar_t* const FILE_CANCEL			= L"S_Cancel";
	const wchar_t* const FILE_DECIDE			= L"S_Decide";
	const wchar_t* const FILE_MOVE				= L"S_CursorMove";
	const wchar_t* const FILE_BGM				= L"B_StageSelect";

	/* ウィンドウ */
	// 描画位置
	constexpr int DRAW_WINDOW_Y = 420;
	// 帯描画位置
	constexpr int DRAW_OBI_Y = 64;
	// 分割線描画位置
	constexpr int DRAW_DIVIDING_LINE_X = 927;
	constexpr int DRAW_DIVIDING_LINE_Y = DRAW_WINDOW_Y;
	/* 画像飾り */
	// 描画位置
	constexpr int DRAW_STAGE_IMAGE_X = 927;
	constexpr int DRAW_STAGE_IMAGE_Y = 281;
	// 飾り位置
	constexpr int DRAW_DECORATIVE_FRAME_X_1 = 1166;
	constexpr int DRAW_DECORATIVE_FRAME_Y_1 = 188;
	constexpr int DRAW_DECORATIVE_FRAME_X_2 = 688;
	constexpr int DRAW_DECORATIVE_FRAME_Y_2 = 375;
	/* ステージフレーム */
	// 描画位置
	constexpr int DRAW_STAGE_X = 160;
	constexpr int DRAW_STAGE_Y = 240;
	// 描画間隔
	constexpr int DRAW_STAGE_INTERVAL = 135;

	/* その他 */
	// ステージ列数
	constexpr int STAGE_LINE_NUM = 4;
	// 選択フレームの追加拡大率
	constexpr float ADD_SCALE_STAGE_SELECT = 0.1f;
	// 選択フレームの拡縮サイクル
	constexpr float CYCLE_SCALING_STAGE_SELECT = 3.0f;
	// 文字ウェーブスピード
	constexpr float WAVE_SPEED = 3.0f;
}

SceneStageSelect::SceneStageSelect() :
	SceneBase(SceneKind::STAGE_SELECT),
	m_selectCurrent(0),
	m_count(0)
{
}

void SceneStageSelect::AsyncInit()
{
	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[I_SELECT_WINDOW_BACK]	= fileMgr.Load(I_SELECT_WINDOW_BACK);
	m_files[I_SELECT_WINDOW]		= fileMgr.Load(I_SELECT_WINDOW);
	m_files[I_RELEASE_FRAME]		= fileMgr.Load(I_RELEASE_FRAME);
	m_files[I_NOT_RELEASE_FRAME]	= fileMgr.Load(I_NOT_RELEASE_FRAME);
	m_files[I_STAGE_SELECT_FRAME]	= fileMgr.Load(I_STAGE_SELECT_FRAME);
	m_files[I_BEST_TIME_FRAME]		= fileMgr.Load(I_BEST_TIME_FRAME);
	m_files[I_DECORATIVE_FRAME]		= fileMgr.Load(I_DECORATIVE_FRAME);
	m_files[I_DIVIDING_LINE]		= fileMgr.Load(I_DIVIDING_LINE);
	m_files[I_SELECT_OBI]			= fileMgr.Load(I_SELECT_OBI);
	m_files[I_COMMON_FRAME]			= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_PAD_A]				= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]				= fileMgr.Load(I_PAD_B);
	m_files[S_CANCEL]				= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]				= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]			= fileMgr.Load(S_CURSOR_MOVE);
	m_files[B_STAGE_SELECT]			= fileMgr.Load(B_STAGE_SELECT);
	auto stageNum = StageDataManager::GetInstance().GetStageNum();
	for (int i = 0; i < stageNum; ++i)
	{
		m_files[I_STAGE_IMAGE_0 + i] = fileMgr.Load(I_STAGE_IMAGE_0 + i);
	}
}

void SceneStageSelect::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_STAGE_SELECT)->GetHandle());
}

void SceneStageSelect::Update(bool isFade)
{
	++m_count;
	if (isFade) return;
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::STAGE_SELECT);
		return;
	}
	SelectUpdate();
}

void SceneStageSelect::End()
{
}

void SceneStageSelect::Draw() const
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	auto& fontMgr = FontManager::GetInstance();
	auto stageNum = stageDataMgr.GetStageNum();
#ifdef _DEBUG
	printf("セーブデータ番号:%d\n", SaveDataManager::GetInstance().GetUseSaveNo() + 1);
#endif
	
	// 背景
	DrawGraph(0, 0, m_files.at(I_SELECT_WINDOW_BACK)->GetHandle(), true);
	// ウィンドウ
	DrawRotaGraphFast(Game::CENTER_X, DRAW_WINDOW_Y, 1.0f, 0.0f, m_files.at(I_SELECT_WINDOW)->GetHandle(), true);
	// 帯
	DrawRotaGraphFast(Game::CENTER_X, DRAW_OBI_Y, 1.0f, 0.0f, m_files.at(I_SELECT_OBI)->GetHandle(), true);
	// 分割線
	DrawRotaGraphFast(DRAW_DIVIDING_LINE_X, DRAW_DIVIDING_LINE_Y, 1.0f, 0.0f, m_files.at(I_DIVIDING_LINE)->GetHandle(), true);
	// ステージ画像
	DrawRotaGraphFast(DRAW_STAGE_IMAGE_X, DRAW_STAGE_IMAGE_Y, 1.0f, 0.0f, m_files.at(I_STAGE_IMAGE_0 + m_selectCurrent)->GetHandle(), true);
	// ステージ画像飾り
	DrawRotaGraphFast(DRAW_DECORATIVE_FRAME_X_1, DRAW_DECORATIVE_FRAME_Y_1, 1.0f, 0.0f, m_files.at(I_DECORATIVE_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_DECORATIVE_FRAME_X_2, DRAW_DECORATIVE_FRAME_Y_2, 1.0f, 180.0f * Game::DEG_2_RAD, m_files.at(I_DECORATIVE_FRAME)->GetHandle(), true);
	// ステージ名描画
	UIUtility::DrawWaveStr(Game::CENTER_X, DRAW_OBI_Y, 0xffffff, stageDataMgr.GetStageName(m_selectCurrent + 1), 36, m_count * WAVE_SPEED, 16);

	// ステージフレーム描画
	DrawStageFrame(stageNum);

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

	// PadUI描画
	int y = DRAW_PAD_Y;
	// A:決定
	DrawRotaGraphFast(DRAW_PAD_FRAME_X, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
	// B:タイトルへ
	y -= DRAW_PAD_Y_INTERVAL;
	DrawRotaGraphFast(DRAW_PAD_FRAME_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_PAD_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X + DRAW_PAD_X_INTERVAL, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"タイトルへ", FONT_SIZE_PAD);
}

void SceneStageSelect::SelectUpdate()
{
	auto& input = Input::GetInstance();
	auto& stageDataMgr = StageDataManager::GetInstance();
	auto& sndMgr = SoundManager::GetInstance();
	auto stageNum = stageDataMgr.GetStageNum() - 2;
	// 行数
	const int STAGE_ROW_NUM = stageNum / STAGE_LINE_NUM;
	// 最後の行の列数
	const int LAST_STAGE_LINE_NUM = stageNum % STAGE_LINE_NUM;
	// 最後の行の列数が自分が定義した列数と同じか
	const bool IS_LAST_SAME_SELF_NUM = LAST_STAGE_LINE_NUM == 0;

	bool isCursorMove = false;
	// 上下移動
	if (input.IsTriggerd(Command::BTN_UP))
	{
		isCursorMove = true;
		m_selectCurrent -= STAGE_LINE_NUM;
		// 1未満になった場合
		if (m_selectCurrent < 0)
		{
			// 最後の行が列数と同じなら
			if (IS_LAST_SAME_SELF_NUM)
			{
				// ステージ数分足す
				m_selectCurrent += stageNum;
			}
			// 同じでないなら
			else
			{
				int row = STAGE_ROW_NUM;
				// 列上で何番目かを取得
				auto temp2 = STAGE_LINE_NUM + m_selectCurrent;
				// 番数が最後の行の列数以上なら行を一つ上にする
				if (temp2 >= LAST_STAGE_LINE_NUM) --row;
				// 計算
				m_selectCurrent = row * STAGE_LINE_NUM + temp2;
			}
		}
	}
	if (input.IsTriggerd(Command::BTN_DOWN))
	{
		isCursorMove = true;
		m_selectCurrent += STAGE_LINE_NUM;
		// ステージ数を超えた場合
		if (m_selectCurrent >= stageNum)
		{
			// カーソルを一番上に戻す
			auto temp = m_selectCurrent - STAGE_LINE_NUM;
			m_selectCurrent = temp % STAGE_LINE_NUM;
		}
	}
	// 左右移動
	if (CursorUtility::CursorUp(m_selectCurrent, stageNum, Command::BTN_LEFT)) isCursorMove = true;
	if (CursorUtility::CursorDown(m_selectCurrent, stageNum, Command::BTN_RIGHT)) isCursorMove = true;
#if false
	// 左右移動(行を変えない)
	if (input.IsTriggerd(Command::BTN_RIGHT))
	{
		// 現在の行での位置を取得
		auto temp = m_selectCurrent % STAGE_LINE_NUM;
		// 最大数を列数にする
		int max = STAGE_LINE_NUM;
		// 最後の行の場合
		if (stageNum - m_selectCurrent < STAGE_LINE_NUM)
		{
			// 最大値を最後の行の列数にする
			max = (stageNum - (m_selectCurrent / STAGE_LINE_NUM) * STAGE_LINE_NUM);
		}
		// 現在の位置を引き
		m_selectCurrent -= temp;
		// その行の列数で計算して
		temp = (temp + 1) % max;
		// 求めた分だけ足す
		m_selectCurrent += temp;
	}
	if (input.IsTriggerd(Command::BTN_LEFT))
	{
		// 現在の行での位置を取得
		auto temp = m_selectCurrent % STAGE_LINE_NUM;
		// 最大数を列数にする
		int max = STAGE_LINE_NUM;
		// 最後の行の場合
		if (stageNum - m_selectCurrent < STAGE_LINE_NUM)
		{
			// 最大値を最後の行の列数にする
			max = (stageNum - (m_selectCurrent / STAGE_LINE_NUM) * STAGE_LINE_NUM);
		}
		// 現在の位置を引き
		m_selectCurrent -= temp;
		// その行の列数で計算して
		temp = (max + temp - 1) % max;
		// 求めた分だけ足す
		m_selectCurrent += temp;
	}
#endif
	if (isCursorMove) sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());

	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		const auto& stageName = stageDataMgr.GetStageName(m_selectCurrent + 1);
		const auto& saveDataMgr = SaveDataManager::GetInstance();
		// ステージ解放されている場合
		if (saveDataMgr.IsReleaseStage(stageName))
		{
			// スタート
			sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle());
			auto next = std::make_shared<SceneMain>(stageName);
			m_scnMgr.Change(next);
		}
		// 解放されていない場合
		else
		{

		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle());
		// タイトルへ
		auto next = std::make_shared<SceneTitle>();
		m_scnMgr.Change(next);
	}
}

void SceneStageSelect::DrawStageFrame(int stageNum) const
{
	int id;
	float scale;
	for (int i = 1; i < stageNum - 1; ++i)
	{
		int x = DRAW_STAGE_X + DRAW_STAGE_INTERVAL * ((i - 1) % STAGE_LINE_NUM);
		int y = DRAW_STAGE_Y + DRAW_STAGE_INTERVAL * ((i - 1) / STAGE_LINE_NUM);

		const auto& stageName = StageDataManager::GetInstance().GetStageName(i);
		bool isSelect = (i == m_selectCurrent + 1);
		// ステージ解放されている場合
		if (SaveDataManager::GetInstance().IsReleaseStage(stageName))
		{
			// 現在選んでいる場合
			if (isSelect)
			{
				id = I_STAGE_SELECT_FRAME;
			}
			// 選んでいない場合
			else
			{
				id = I_RELEASE_FRAME;
			}
		}
		// ステージ解放されていない場合
		else
		{
			id = I_NOT_RELEASE_FRAME;
		}
		// 選択中の場合
		if (isSelect)
		{

			scale = 1.0f + std::sin(m_count * Game::DEG_2_RAD * CYCLE_SCALING_STAGE_SELECT) * ADD_SCALE_STAGE_SELECT;
		}
		// 未選択の場合
		else
		{
			scale = 1.0f;
		}
		DrawRotaGraphFast(x, y, scale, 0.0f, m_files.at(id)->GetHandle(), true);
	}
}
