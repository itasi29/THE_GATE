#pragma once
#include "SceneBase.h"
#include <array>
#include <memory>

class StageManager;
class AnimController;

class SceneResult : public SceneBase
{
private:
	enum class DecideTime
	{
		DEC,
		SEC,
		MIN,
		MAX
	};

public:
	SceneResult(const wchar_t* const stageName, const int clearTime);
	~SceneResult() {}

	// シーンに入るときの初期化処理(非同期)
	void AsyncInit() override;
	// シーンに入るときの初期化処理(同期)
	void Init() override;
	// シーンを抜けるときの処理
	void End() override;
	// 更新処理(同期)
	void Update(bool isFade) override;
	// 描画処理
	void Draw() const override;

private:
	void TimeUpdate();
	void RankUpdate();
	void RankingUpdate();
	void PadUIFadeUpdate();
	void SelectUpdate();

	void DrawTime() const;
	void DrawTime(int x, int y, DecideTime kind, int time, unsigned int color) const;
	void DrawRank() const;
	void DrawRankingUpdate() const;
	void DrawPadUI() const;

	void DrawStamp(int x, int y, int fileId, float defSize, float largeSize, float largeSize2, float smallSize, int count, bool isStamped) const;

private:
	const wchar_t* const m_stageName;
	const int m_clearTime;

	using ProcessUpdate_t = void(SceneResult::*)();
	ProcessUpdate_t m_processFunc;

	std::shared_ptr<StageManager> m_stageMgr;
	std::shared_ptr<AnimController> m_anim;

	std::array<int, static_cast<size_t>(DecideTime::MAX)> m_moveCount[3];
	int m_indexDecide;
	int m_count;
	int m_vsH;
	int m_psH;

	std::array<bool, static_cast<size_t>(DecideTime::MAX)> m_isDecide[3];
	std::array<bool, static_cast<size_t>(DecideTime::MAX)> m_isMove[3];
	bool m_isAllDecide;
	bool m_isAllMoved;
	bool m_isRankStamped;
	bool m_isRankingStamped;
	bool m_isRankingUpdate;
};

