#pragma once
#include <string>

class Novel
{
public:
	Novel();
	~Novel();

	void Start(const std::wstring& str);
	void Skip();

	void Update(const int interval);
	void Draw(int spaceWidthNum, int startY, unsigned int color) const;
	void Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const;
	
	/// <summary>
	/// 終了フラグ
	/// </summary>
	/// <returns>true: 終了/ false: 描画中</returns>
	bool IsEnd() const { return m_isEnd; }

private:
	std::wstring m_str;

	int m_frame;
	// 現在描画する位置
	int m_current;

	bool m_isEnd;
};

