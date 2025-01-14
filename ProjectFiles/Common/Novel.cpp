#include "Novel.h"
#include <DxLib.h>
#include "Game.h"
#include "FontManager.h"

namespace
{
	// 文字間隔
	constexpr int INTERVAL_X = 16;
	constexpr int INTERVAL_Y = 16;
	// 描画可能文字列
	constexpr int NUM_DRAW_BASE = Game::WINDOW_W / INTERVAL_X;
	// ベースフォントサイズ
	constexpr int BASE_FONT_SIZE = 16;
}

Novel::Novel() :
	m_frame(0),
	m_current(1),
	m_isEnd(false)
{
}

Novel::~Novel()
{
}

void Novel::Start(const std::wstring& str)
{
	m_str = str;
	// 初期化
	m_current = 1;
	m_frame = 0;
	m_isEnd = false;
}

void Novel::Skip()
{
	if (m_isEnd) return;

	m_current = static_cast<int>(m_str.size());
	m_isEnd = true;
}

void Novel::Update(const int interval)
{
	if (m_isEnd) return;

	++m_frame;
	if (m_frame > interval)
	{
		m_frame = 0;
		++m_current;
		if (m_current >= m_str.size())
		{
			m_isEnd = true;
		}
	}
}

void Novel::Draw(int spaceWidthNum, int startY, unsigned int color) const
{
	// 描画可能文字列数
	const int num = NUM_DRAW_BASE - spaceWidthNum * 2;
	// 空白サイズ
	const int space = INTERVAL_X * spaceWidthNum;

	// 1文字ずつ描画
	for (int i = 0; i < m_current; ++i)
	{
		const auto& c = m_str.at(i);
		const int x = (i % num) * INTERVAL_X + space;
		const int y = (i / num) * INTERVAL_Y + startY;
		DrawFormatString(x, y, color, L"%c", c);
	}
}

void Novel::Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const
{
	// 文字列ハンドル
	const auto fontH = FontManager::GetInstance().GetHandle(fontSize);

	const float rate = fontSize / static_cast<float>(BASE_FONT_SIZE);
	const int intervalX = static_cast<int>(INTERVAL_X * rate * rateW);
	const int intervalY = static_cast<int>(INTERVAL_Y * rate * rateH);

	// 描画可能文字列数
	const int num = Game::WINDOW_W / intervalX - spaceWidthNum * 2;
	// 空白サイズ
	const int space = intervalX * spaceWidthNum;

	// 1文字ずつ描画
	for (int i = 0; i < m_current; ++i)
	{
		const auto& c = m_str.at(i);
		const int x = (i % num) * intervalX + space;
		const int y = (i / num) * intervalY + startY;
		DrawFormatStringToHandle(x, y, color, fontH, L"%c", c);
	}
}
