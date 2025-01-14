#include "SceneOption.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "Game.h"
#include "Files.h"
#include "SceneManager.h"
#include "SceneStageSelect.h"
#include "SceneMain.h"
#include "SoundManager.h"

namespace
{
	// 選択種類
	enum class SelectTitleCurrent : int
	{
		BGM,
		SE,
		CLOSE,
		MAX,
	};
	enum class SelectMainCurrent : int
	{
		BGM,
		SE,
		RESTART,
		RETURN_STAGE_SELECT,
		MAX
	};
	// 確認種類
	enum class CheckMainCurrent : int
	{
		CANCEL,
		RETURN,
		MAX
	};
	// 音変更
	enum class SoundChange
	{
		BGM,
		SE,
	};

	// フレーム描画位置
	constexpr int DRAW_FRAME_X = 256;
	constexpr int DRAW_FRAME_Y = 200;
	// フレーム描画間隔
	constexpr int DRAW_FRAME_Y_INTERVAL = 126;
	// フレーム画像サイズ
	constexpr float FILE_SIZE_FRAME = 1.125f;
	// サウンドバー描画位置
	constexpr int DRAW_SOUND_X = 725;
	constexpr int DRAW_SOUND_Y = DRAW_FRAME_Y;
	// サウンドバー描画間隔
	constexpr int DRAW_SOUND_Y_INTERVAL = DRAW_FRAME_Y_INTERVAL;
	// サウンドポイント描画位置
	constexpr int DRAW_SOUND_POINT_X = 454;
	// サウンドポイント描画間隔
	constexpr int DRAW_SOUND_POINT_X_INTERVAL = 542;
	// フォントサイズ
	constexpr int FONT_SIZE = 56;
	// 選択文字カラー
	constexpr unsigned int COLOR_SELECT = 0x000000;
	// 未選択文字カラー
	constexpr unsigned int COLOR_NOT_SELECT = 0xffffff;
	// タイトルから来た時の文字列
	const std::vector<std::wstring> STRS_TITLE =
	{
		L"ＢＧＭ",
		L"ＳＥ",
		L"閉じる"
	};
	// メインからの来た時の文字列
	const std::vector<std::wstring> STRS_MAIN =
	{
		L"ＢＧＭ",
		L"ＳＥ",
		L"リスタート",
		L"ステージ選択へ",
	};
	// メインから来た時の確認文字列
	const std::vector<std::wstring> STRS_CHECK_MAIN =
	{
		L"キャンセル",
		L"ステージ選択へ"
	};
	const wchar_t* const STR_MAIN_CHECK = L"ステージ選択に戻りますか？";
	constexpr unsigned int COLOR_CHECK = 0xffffff;
	constexpr int DRAW_CHECK_STR_X = Game::CENTER_X;
	constexpr int DRAW_CHECK_STR_Y = 200;
	constexpr int DRAW_FRAME_CHECK_X = 360;
	constexpr int DRAW_FRAME_CHECK_Y = 500;
	constexpr int DRAW_FRAME_CHEKC_X_INTERVAL = 560;
	constexpr float FILE_SIZE_FRAME_CHECK = 1.125f;
	constexpr int FONT_SIZE_CHECK = 56;
	constexpr int FONT_SIZE_ALERT = 80;

	/* サウンドバー */
	// フレーム描画位置
	constexpr int DRAW_SOUND_TXT_FRAME_X = 1100;
	constexpr int DRAW_SOUND_TXT_FRAME_SUB_Y = 0;
	// フレーム画像サイズ
	constexpr float FILE_SIZE_SOUND_TXT_FRAME_W = 0.5f;
	constexpr float FILE_SIZE_SOUND_TXT_FRAME_H = 0.875f;
	// 文字描画位置
	constexpr int DRAW_SOUND_TXT_X = DRAW_SOUND_TXT_FRAME_X - 24;
	constexpr int DRAW_SOUND_TXT_SUB_Y = 24;
	// 文字フォントサイズ
	constexpr int FONT_SIZE_SOUND = 48;
	// 文字カラー
	constexpr unsigned int COLOR_SOUND = 0xffffff;

	/* PadUi描画 */
	constexpr int DRAW_PAD_X = 1000;		// 基準Y描画位置
	constexpr int DRAW_PAD_Y = 650;			// 画像描画位置
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int DRAW_PAD_FRAME_X = 1060;			// フレーム描画位置
	constexpr float FILE_SIZE_PAD_FRAME = 0.525f;	// フレームサイズ
	constexpr int DRAW_PAD_STR_X = 1020;	// 文字列描画位置
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int PAD_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー
	// 描画間隔
	constexpr int DRAW_PAD_X_INTERVAL = 50;
	constexpr int DRAW_PAD_Y_INTERVAL = 50;

	// 変更音量
	constexpr int CHANGE_VOLUME = 1;
	// サウンド更新を行う番号のマックス値
	constexpr int SOUND_CHECK_CURRENT_MAX = 2;
	// 文字のウェーブサイズ
	constexpr int WAVE_SIZE = 16;
	// ウェーブスピード
	constexpr float WAVE_SPEED = 3.0f;
	// リピート間隔
	constexpr int REPEATE_INTERVAL = 1;
	// スタートリピート間隔
	constexpr int START_REPEATE_INTERVAL = 10;
	// フェードスピード
	constexpr float FADE_SPEED = 0.1f;
	// フェード時の画面外の値
	constexpr int DRAW_OUT_VAL = 360;
}

SceneOption::SceneOption() :
	SceneBase(SceneKind::OPTION),
	m_updateFunc(&SceneOption::FadeInUpdate),
	m_drawFunc(&SceneOption::DrawFade),
	m_sceneMain(nullptr),
	m_openScene(SceneKind::TITLE),
	m_preKind(RepeatKind::NONE),
	m_selectCurrent(0),
	m_checkCurrent(0),
	m_waveCount(0),
	m_fadeCount(0.0f)
{
}

void SceneOption::AsyncInit()
{
}

void SceneOption::Init()
{
	// ファイルロード
	auto& fileMgr = FileManager::GetInstance();
	m_files[I_OPTION_WINDOW]		= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_COMMON_FRAME]			= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_COMMON_SELECT_FRAME]	= fileMgr.Load(I_COMMON_SELECT_FRAME);
	m_files[I_MAIN_SOUND_BAR]		= fileMgr.Load(I_MAIN_SOUND_BAR);
	m_files[I_VOLUME_POINT]			= fileMgr.Load(I_VOLUME_POINT);
	m_files[I_PAD_A]				= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]				= fileMgr.Load(I_PAD_B);
	m_files[S_CANCEL]				= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]				= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]			= fileMgr.Load(S_CURSOR_MOVE);
	// 文字列登録
	m_selectStrList[SceneKind::TITLE]			= STRS_TITLE;
	m_selectStrList[SceneKind::STAGE_SELECT]	= STRS_TITLE;
	m_selectStrList[SceneKind::MAIN]			= STRS_MAIN;
}

void SceneOption::End()
{
	// サウンドデータの保存
	SoundManager::GetInstance().Save();
}

void SceneOption::Update(bool isFade)
{
	m_waveCount++;
	if (isFade) return;
	(this->*m_updateFunc)();
}

void SceneOption::Draw() const
{
	(this->*m_drawFunc)();
	// PadUI描画
	int y = DRAW_PAD_Y;
	// A:決定
	DrawRotaGraphFast(DRAW_PAD_FRAME_X, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
	// B:閉じる
	y -= DRAW_PAD_Y_INTERVAL;
	DrawRotaGraphFast(DRAW_PAD_FRAME_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD_FRAME, 0.0f, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_PAD_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X + DRAW_PAD_X_INTERVAL, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"閉じる", FONT_SIZE_PAD);
}

void SceneOption::CheckOpenScene(SceneKind openScene)
{
	m_openScene = openScene;
	if (openScene == SceneKind::TITLE || openScene == SceneKind::STAGE_SELECT)
	{
		m_uiList = UIMoveData::Make(static_cast<int>(SelectTitleCurrent::MAX), DRAW_FRAME_X, DRAW_FRAME_Y, 0, DRAW_FRAME_Y_INTERVAL);
	}
	else if (openScene == SceneKind::MAIN)
	{
		m_uiList = UIMoveData::Make(static_cast<int>(SelectMainCurrent::MAX), DRAW_FRAME_X, DRAW_FRAME_Y, 0, DRAW_FRAME_Y_INTERVAL);
		
		m_checkUiList = UIMoveData::Make(static_cast<int>(CheckMainCurrent::MAX), DRAW_FRAME_CHECK_X, DRAW_FRAME_CHECK_Y, DRAW_FRAME_CHEKC_X_INTERVAL, 0);
		m_checkStr = STR_MAIN_CHECK;
	}
	else
	{
		assert(false && "開くシーンがおかしいです");
	}
}

void SceneOption::FadeInUpdate()
{
	// フェード更新
	m_fadeCount += FADE_SPEED;
	// フェード中は処理終了
	if (m_fadeCount < 1.0f) return;
	
	m_fadeCount = 1.0f;
	m_drawFunc = &SceneOption::DrawCommon;
	// タイトルの時のアップデート
	if (m_openScene == SceneKind::TITLE || m_openScene == SceneKind::STAGE_SELECT)
	{
		m_updateFunc = &SceneOption::SelectTitleUpdate;
	}
	// メインの時のアップデート
	else if (m_openScene == SceneKind::MAIN)
	{
		m_updateFunc = &SceneOption::SelectMainUpdate;
	}
}

void SceneOption::FadeOutUpdate()
{
	// フェード行進
	m_fadeCount -= FADE_SPEED;
	// フェード終了したらオプションを閉じる
	if (m_fadeCount < 0) m_scnMgr.CloseOption();
}

void SceneOption::SelectTitleUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	bool isCursorMove = false;
	if (CursorUtility::CursorUp<SelectTitleCurrent>(m_selectCurrent, SelectTitleCurrent::MAX)) isCursorMove = true;
	if (CursorUtility::CursorDown<SelectTitleCurrent>(m_selectCurrent, SelectTitleCurrent::MAX)) isCursorMove = true;
	if (isCursorMove)
	{
		sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}
	// BGM・SE調整
	if (m_selectCurrent < SOUND_CHECK_CURRENT_MAX)
	{
		SoundChangeUpdate(m_selectCurrent);
	}

	auto& input = Input::GetInstance();
	bool isClose = false;
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// 閉じる
		if (m_selectCurrent == static_cast<int>(SelectTitleCurrent::CLOSE))
		{
			isClose = true;
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// 閉じる
		isClose = true;
	}

	if (isClose)
	{
		m_updateFunc = &SceneOption::FadeOutUpdate;
		m_drawFunc = &SceneOption::DrawFade;
	}
}

void SceneOption::SelectMainUpdate()
{
	if (CursorUtility::CursorUp<SelectMainCurrent>(m_selectCurrent, SelectMainCurrent::MAX)) m_waveCount = 0;
	if (CursorUtility::CursorDown<SelectMainCurrent>(m_selectCurrent, SelectMainCurrent::MAX)) m_waveCount = 0;
	// BGM・SE調整
	if (m_selectCurrent < SOUND_CHECK_CURRENT_MAX)
	{
		SoundChangeUpdate(m_selectCurrent);
	}

	auto& input = Input::GetInstance();
	bool isClose = false;
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		// チェックポイントから再開
		if (m_selectCurrent == static_cast<int>(SelectMainCurrent::RESTART))
		{
			m_sceneMain->OnRestart();
			isClose = true;
		}
		// ステージ選択に戻る
		else if (m_selectCurrent == static_cast<int>(SelectMainCurrent::RETURN_STAGE_SELECT))
		{
			m_updateFunc = &SceneOption::CheckMainUpdate;
			m_drawFunc = &SceneOption::DrawCheck;
			m_checkCurrent = 0;
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		// 閉じる
		isClose = true;
	}

	if (isClose)
	{
		m_updateFunc = &SceneOption::FadeOutUpdate;
		m_drawFunc = &SceneOption::DrawFade;
	}
}

void SceneOption::CheckMainUpdate()
{
	if (CursorUtility::CursorUp<CheckMainCurrent>(m_checkCurrent, CheckMainCurrent::MAX, Command::BTN_LEFT)) m_waveCount = 0;
	if (CursorUtility::CursorDown<CheckMainCurrent>(m_checkCurrent, CheckMainCurrent::MAX, Command::BTN_RIGHT)) m_waveCount = 0;

	auto& input = Input::GetInstance();
	bool isCancel = false;
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		// ステージ選択シーンへ
		if (m_checkCurrent == static_cast<int>(CheckMainCurrent::RETURN))
		{
			auto next = std::make_shared<SceneStageSelect>();
			m_scnMgr.Change(next);
			m_updateFunc = &SceneOption::FadeOutUpdate;
			m_drawFunc = &SceneOption::DrawFade;
		}
		// キャンセル
		else if (m_checkCurrent == static_cast<int>(CheckMainCurrent::CANCEL))
		{
			isCancel = true;
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		isCancel = true;
	}

	if (isCancel)
	{
		m_updateFunc = &SceneOption::SelectMainUpdate;
		m_drawFunc = &SceneOption::DrawCommon;
	}
}

void SceneOption::SoundChangeUpdate(int current)
{
	auto& input = Input::GetInstance();
	auto& sndMgr = SoundManager::GetInstance();

	// それぞれに合った方の現在のボリュームを取得
	int volume = 0;
	if (current == static_cast<int>(SoundChange::BGM))
	{
		volume = sndMgr.GetBgmVol();
	}
	else if (current == static_cast<int>(SoundChange::SE))
	{
		volume = sndMgr.GetSeVol();
	}

	bool isChange = false;
	RepeatKind kind;
	// 音量減少
	if (input.IsRepeat(Command::BTN_LEFT, kind, REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		volume -= CHANGE_VOLUME;
		isChange = true;
	}
	// 上をリピートしていない場合、音量増加
	else if (kind == RepeatKind::NONE && input.IsRepeat(Command::BTN_RIGHT, kind, REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		volume += CHANGE_VOLUME;
		isChange = true;
	}

	// 変更していない時
	if (!isChange)
	{
		// ボリューム変更を行っているのがSEの時
		if (current == static_cast<int>(SoundChange::SE))
		{
			// リピートから離した時なら音を鳴らす
			const bool isReverse = m_preKind == RepeatKind::COOL_DOWN && kind == RepeatKind::NONE;
			if (isReverse)
			{
				sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
			}
		}
	}
	// 変更している時
	else
	{
		// それぞれに合わせて変更したボリュームを設定
		if (current == static_cast<int>(SoundChange::BGM))
		{
			sndMgr.ChangeBgmVol(volume);
		}
		else if (current == static_cast<int>(SoundChange::SE))
		{
			sndMgr.ChangeSeVol(volume, kind == RepeatKind::TRIGGER);
		}
	}
	m_preKind = kind;
}

void SceneOption::DrawFade() const
{
	int y = Easing::Lerp(-DRAW_OUT_VAL, Game::CENTER_Y, m_fadeCount);
	DrawRotaGraphFast(Game::CENTER_X, y, 1.0f, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
}

void SceneOption::DrawCommon() const
{
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	UIUtility::DrawFrameAndStr(m_uiList, FILE_SIZE_FRAME, 0.0f, FONT_SIZE, m_selectCurrent, m_selectStrList.at(m_openScene), selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, WAVE_SIZE, m_waveCount * WAVE_SPEED);
	auto& sndMgr = SoundManager::GetInstance();
	DrawSoundBar(DRAW_SOUND_Y, sndMgr.GetBgmVolRate());
	DrawSoundBar(DRAW_SOUND_Y + DRAW_SOUND_Y_INTERVAL, sndMgr.GetSeVolRate());
}

void SceneOption::DrawSoundBar(int y, float volumeRate) const
{
	DrawRotaGraphFast(DRAW_SOUND_X, y, 1.0f, 0.0f, m_files.at(I_MAIN_SOUND_BAR)->GetHandle(), true);
	int x = DRAW_SOUND_POINT_X + static_cast<int>(DRAW_SOUND_POINT_X_INTERVAL * volumeRate);
	DrawRotaGraphFast(x, y, 1.0f, 0.0f, m_files.at(I_VOLUME_POINT)->GetHandle(), true);
	y -= DRAW_SOUND_TXT_FRAME_SUB_Y;
	int w, h;
	GetGraphSize(m_files.at(I_COMMON_FRAME)->GetHandle(), &w, &h);
	w = static_cast<int>(w * 0.5f * FILE_SIZE_SOUND_TXT_FRAME_W);
	h = static_cast<int>(h * 0.5f * FILE_SIZE_SOUND_TXT_FRAME_H);
	DrawExtendGraph(DRAW_SOUND_TXT_FRAME_X - w, y - h, DRAW_SOUND_TXT_FRAME_X + w, y + h, m_files.at(I_COMMON_FRAME)->GetHandle(), true);
	DrawFormatStringToHandle(DRAW_SOUND_TXT_X, y - DRAW_SOUND_TXT_SUB_Y, COLOR_SOUND, FontManager::GetInstance().GetHandle(FONT_SIZE_SOUND), L"%.1f", 100 * volumeRate);
}

void SceneOption::DrawCheck() const
{
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	int x = UIUtility::GetSubCenterFromLen(DRAW_CHECK_STR_X, STR_MAIN_CHECK, FONT_SIZE_ALERT);
	DrawStringToHandle(x, DRAW_CHECK_STR_Y, STR_MAIN_CHECK, COLOR_CHECK, FontManager::GetInstance().GetHandle(FONT_SIZE_ALERT));
	UIUtility::DrawFrameAndStr(m_checkUiList, FILE_SIZE_FRAME_CHECK, 0.0f, FONT_SIZE_CHECK, m_checkCurrent, STRS_CHECK_MAIN, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, WAVE_SIZE, m_waveCount * WAVE_SPEED);
}
