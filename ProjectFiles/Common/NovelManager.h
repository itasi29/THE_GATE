#pragma once
#include <vector>
#include <string>
#include <memory>

class Novel;

/// <summary>
/// ノベルゲームのように文字を出力管理するクラス
/// </summary>
class NovelManager final
{
public:
	enum class State
	{
		DRAWING,
		WAIT,
		END,
	};

private:
	struct Info
	{
		std::wstring str = L"";
		int interval = -1;
		int nextId = -1;
		bool isNext = false;
	};

private:
	NovelManager();
	~NovelManager();

	NovelManager(const NovelManager&) = delete;
	NovelManager(const NovelManager&&) = delete;
	NovelManager operator= (const NovelManager&) = delete;
	NovelManager operator= (const NovelManager&&) = delete;

public:
	static NovelManager& GetInstance();

	/// <summary>
	/// 文字列データの読み込み
	/// </summary>
	void Load();
	/// <summary>
	/// 文字列の開始
	/// </summary>
	/// <param name="id">使用文字列ID</param>
	void Start(int id);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画(フォントなし)
	/// </summary>
	/// <param name="spaceWidthNum">左右空き文字数</param>
	/// <param name="startY">スタートY座標</param>
	/// <param name="color">文字カラー</param>
	void Draw(int spaceWidthNum, int startY, unsigned int color) const;
	/// <summary>
	/// 描画(フォントあり)
	/// </summary>
	/// <param name="spaceWidthNum">左右空き文字数</param>
	/// <param name="startY">スタートY座標</param>
	/// <param name="color">文字カラー</param>
	/// <param name="fontSize">フォントサイズ</param>
	/// <param name="rateW">左右の詰め具合(def = 0.8f)</param>
	/// <param name="rateH">上下の詰め具合(def = 1.0f)</param>
	void Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW = BASE_RATE_W_FONT, float rateH = BASE_RATE_H_FONT) const;


	bool IsNextMove() const { return m_isNextMove; }
	/// <summary>
	/// 終了判定
	/// ※次の文字列があるものの場合はfalseを返す
	/// </summary>
	/// <returns>true: 全文字描画/ false: 描画途中or全文字描画終了したが次の文字列がある</returns>
	bool IsEnd() const;
	/// <summary>
	/// 現在の状態を取得
	/// </summary>
	/// <returns>DRAWING:描画中, WAIT:全文字描画して、次の文字列描画まで待機中, END:全文字描画</returns>
	State GetState() const;

public:
	static constexpr float BASE_RATE_W_FONT = 0.8f;
	static constexpr float BASE_RATE_H_FONT = 1.0f;

private:
	// 情報リスト
	std::vector<Info> m_infoList;
	// 描画データ
	std::shared_ptr<Novel> m_novel;
	// 現在のID
	int m_nowId;
	// 確認フラグ
	bool m_isNowStart;
	bool m_isStop;
	bool m_isNextMove;
};

