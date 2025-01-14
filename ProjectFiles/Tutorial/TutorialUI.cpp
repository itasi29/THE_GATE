#include "TutorialUI.h"
#include <unordered_map>
#include "TutorialManager.h"
#include "CommandList.h"
#include "Input.h"
#include "Files.h"
#include "Novels.h"
#include "UIUtility.h"

namespace
{
	enum class FileIndex
	{
		BUTTON,
		TEXT_FRAME,
		IMAGE,
	};

	
}

TutorialUI::TutorialUI() :
	m_novelId(-1),
	m_nowFileIndex(0),
	m_count(0),
	m_waveFrame(0),
	m_isEnd(false)
{
}

TutorialUI::~TutorialUI()
{
}

void TutorialUI::Init(const TutorialData& data)
{
	// ノベルデータ読み込み
	NovelManager::GetInstance().Start(data.novelId);
	m_novelId = data.novelId;

	// 画像読み込み
	auto& file = FileManager::GetInstance();
	m_files[static_cast<int>(FileIndex::BUTTON)] = file.Load(I_PAD_A);
	m_files[static_cast<int>(FileIndex::TEXT_FRAME)] = file.Load(I_SELECT_OBI);
	for (int i = 0; i < data.fileNum; ++i)
	{
		auto& info = data.fileInfo.at(i);

		m_files[static_cast<int>(FileIndex::IMAGE) + i] = file.Load(info.t2);
	}
	m_fileInfo = data.fileInfo;

	// 初期化
	m_nowFileIndex = 0;
	m_count = 0;
	m_waveFrame = 0;
	m_isEnd = false;
}

void TutorialUI::End()
{
}

void TutorialUI::Update()
{
	auto& novel = NovelManager::GetInstance();
	auto& input = Input::GetInstance();

	// OK押した瞬間かつテキストが終了していればチュートリアル終了
	if (novel.IsEnd() && input.IsTriggerd(Command::BTN_OK))
	{
		m_isEnd = true;
		TutorialManager::GetInstance().End();
		return;
	}
	novel.Update();
	// 次のテキストに移行したら画像も更新させる
	if (novel.IsNextMove())
	{
		++m_count;
		if (m_count > m_fileInfo.at(m_nowFileIndex).t1)
		{
			++m_nowFileIndex;
		}
	}
	// 文字のウェーブ更新
	++m_waveFrame;
}

void TutorialUI::Draw() const
{
	constexpr int DRAW_IMAGE_X = Game::CENTER_X;
	constexpr int DRAW_IMAGE_Y = 250;

	constexpr int DRAW_FRAME_X = Game::CENTER_X;
	constexpr int DRAW_FRAME_Y = 620;
	constexpr float FILE_SIZE_FRAME = 2.0f;

	constexpr int DRAW_BUTTON_X = 1000;
	constexpr int DRAW_BUTTON_Y = 670;
	constexpr float FILE_SIZE_BUTTON = 0.5f;

	constexpr int DRAW_BUTTON_TEXT_X = 1040;
	constexpr int DRAW_BUTTON_TEXT_Y = DRAW_BUTTON_Y;
	constexpr int FONT_SIZE_NEXT = 24;
	constexpr unsigned int COLOR_BUTTON_TEXT = 0xffffff;
	constexpr float WAVE_SPEED = 2.0f;
	constexpr int UP_SIZE = 8;

	constexpr int DRAW_TEXT_START_Y = 580;
	constexpr int DRAW_TEXT_SPACE_NUM = 4;
	constexpr int FONT_SIZE_TEXT = 32;
	constexpr unsigned int COLOR_TEXT = 0xffffff;

	const int imageH  = m_files.at(static_cast<int>(FileIndex::IMAGE) + m_nowFileIndex)->GetHandle();
	const int buttonH = m_files.at(static_cast<int>(FileIndex::BUTTON))->GetHandle();
	const int frameH  = m_files.at(static_cast<int>(FileIndex::TEXT_FRAME))->GetHandle();

	const auto state = NovelManager::GetInstance().GetState();

	DrawRotaGraph(DRAW_IMAGE_X, DRAW_IMAGE_Y, 1.0f, 0.0f, imageH, true);
	DrawRotaGraph(DRAW_FRAME_X, DRAW_FRAME_Y, FILE_SIZE_FRAME, 0.0f, frameH, true);
	NovelManager::GetInstance().Draw(DRAW_TEXT_SPACE_NUM, DRAW_TEXT_START_Y, COLOR_TEXT, FONT_SIZE_TEXT);
	DrawRotaGraph(DRAW_BUTTON_X, DRAW_BUTTON_Y, FILE_SIZE_BUTTON, 0.0f, buttonH, true);
	std::wstring buttonTxt;
	if (state == NovelManager::State::DRAWING)		buttonTxt = L"スキップ";
	else if (state == NovelManager::State::WAIT)	buttonTxt = L"次のテキストへ";
	else if (state == NovelManager::State::END)		buttonTxt = L"チュートリアルを閉じる";
	UIUtility::DrawWaveStrLeft(DRAW_BUTTON_TEXT_X, DRAW_BUTTON_TEXT_Y, COLOR_BUTTON_TEXT, buttonTxt.c_str(), FONT_SIZE_NEXT, m_waveFrame * WAVE_SPEED, UP_SIZE);
}
