#include "NovelManager.h"
#include <DxLib.h>
#include "Input.h"
#include "CommandList.h"
#include "Novel.h"
#include "FileManager.h"
#include "Utility/StringUtility.h"

namespace
{
	const wchar_t* const PATH = L"Data/Master/NovelData.dat";
	enum class FileIndex
	{
		NEXT_FLAG,
		NEXT_ID,
		INTERVAL,
		STR
	};
}

NovelManager::NovelManager() :
	m_novel(std::make_shared<Novel>()),
	m_nowId(-1),
	m_isNowStart(false),
	m_isStop(true),
	m_isNextMove(false)
{
}

NovelManager::~NovelManager()
{
}


NovelManager& NovelManager::GetInstance()
{
	static NovelManager instance;
	return instance;
}

void NovelManager::Load()
{
	SetUseASyncLoadFlag(false);
	auto handle = FileRead_open(PATH);
	if (handle == 0)
	{
		assert(false && "ファイルのオープンに失敗しました(Novel)");
		return;
	}

	int size = 0;
	FileRead_read(&size, sizeof(int), handle);
	for (int i = 0; i < size; ++i)
	{
		Info info;

		// 連続フラグ
		int isNext = 0;
		FileRead_read(&isNext, sizeof(int), handle);
		info.isNext = static_cast<bool>(isNext);
		// 連続ID
		if (info.isNext)
		{
			int nextId = 0;
			FileRead_read(&nextId, sizeof(int), handle);
			info.nextId = nextId;
		}
		// 描画間隔
		int interval = 0;
		FileRead_read(&interval, sizeof(int), handle);
		info.interval = interval;
		// 文字列
		int len;
		void* buf;
		FileRead_read(&len, sizeof(int), handle);
		buf = new char[len * 2];
		FileRead_read(buf, len * 2, handle);
		info.str = StringUtility::Utf8ToWString(static_cast<char*>(buf), len);
		delete[] buf;

		// 追加
		m_infoList.push_back(info);
	}

	FileRead_close(handle);
	SetUseASyncLoadFlag(true);
}

void NovelManager::Start(int id)
{
	m_novel->Start(m_infoList.at(id).str);
	m_nowId = id;
	m_isStop = false;
	m_isNowStart = true;
}

void NovelManager::Update()
{
	m_isNextMove = false;
	if (m_isStop) return;

	const auto& info = m_infoList.at(m_nowId);
	m_novel->Update(info.interval);


	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OK))
	{
		// 終了していない場合
		if (!m_novel->IsEnd())
		{
			// 今回初めていない場合スキップする
			if (!m_isNowStart) m_novel->Skip();
		}
		// 終了していて、次の文字列がある場合
		else if (info.isNext)
		{
			// 次の文章を開始する
			Start(info.nextId);
			m_isNextMove = true;
		}
		// 終了していて、次がない場合
		else
		{
			// 停止する
			m_isStop = true;
		}
	}

	m_isNowStart = false;
}

void NovelManager::Draw(int spaceWidthNum, int startY, unsigned int color) const
{
	if (m_isStop) return;
	m_novel->Draw(spaceWidthNum, startY, color);
}

void NovelManager::Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const
{
	if (m_isStop) return;
	m_novel->Draw(spaceWidthNum, startY, color, fontSize, rateW, rateH);
}

bool NovelManager::IsEnd() const
{
	// ストップなら終了していることに
	if (m_isStop) return true;

	const auto& info = m_infoList.at(m_nowId);
	// 次の文字列がある場合は終了していないことに
	if (info.isNext) return false;
	// 現在の状態を返す
	return m_novel->IsEnd();
}

NovelManager::State NovelManager::GetState() const
{
	const auto& info = m_infoList.at(m_nowId);
	// 終了していなければ描画中
	if (!m_novel->IsEnd())	return State::DRAWING;
	// 次の文字列があるので待機中
	if (info.isNext)		return State::WAIT;
	// 終了
	else					return State::END;
}
