#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>
#include <variant>
#include "Vec3.h"

class GateManager;
class StageManager;
class CameraManager;
class Player;
enum class PlayerState;

// チュートリアルの種類
enum class TutorialKind
{
	STATE,
	INPUT,
	CP,
};
// プレイヤーステートでのチェック情報
struct CheckStateInfo
{
	PlayerState state;
	int frame = 0;
};
// 入力情報でのチェック情報
struct CheckInputInfo
{
	enum class TriggerType
	{
		STICK_LEFT,
		STICK_RIGHT,
		TRIGGER,
	};
	bool isTrigger = false;
	TriggerType type = TriggerType::STICK_RIGHT;
	std::string command;
	int frame = 0;
};
// CPでのチェック情報
struct CheckCPInfo
{
	int cp = -1;
};
// 共用体に
using CheckInfo = std::variant<CheckStateInfo, CheckInputInfo, CheckCPInfo>;
// チュートリアルで使う情報
struct TutorialInfo
{
	TutorialKind kind = TutorialKind::STATE;
	CheckInfo info;
	bool isDrawClear = false;
	bool isDrawDestination = false;
	Vec3 destination;
	bool isDrawGoal = false;
	int goalTextId = -1;
	bool isDrawNovel = false;
	int novelId = -1;
};

class SceneTutorial : public SceneBase
{
private:
	

	struct CBuff
	{
		float frame;
		float fireRed;
		float fireGreen;
		float fireBlue;
	};
	
public:
	SceneTutorial();
	~SceneTutorial();

	// 初期化処理(非同期)
	void AsyncInit() override;
	// 初期化処理(同期)
	void Init() override;
	// 終了処理
	void End() override;
	// 更新処理(同期)
	void Update(bool isFade) override;
	// 描画処理
	void Draw() const override;

	void OnRestart();

private:
	void Load();

	void MainUpdate();
	void RestartUpdate();

	void CheckStateUpdate();
	void CheckInputUpdate();
	void CheckCPUpdate();
	void ClearTutorialUpdate();

	void OnClearTutorial();
	void ChangeTutorial();

	void DrawMain() const;
	void DrawRestart() const;

	void DrawUI() const;
	void DrawGateUI() const;
	void DrawTutorialClear() const;
	void DrawTutorialGoal() const;
	void DrawTutorialNovel() const;
	void DrawTutorialDestination() const;

private:
	using UpdateFunc_t = void(SceneTutorial::*)();
	using DrawFunc_t = void(SceneTutorial::*)() const;
	UpdateFunc_t m_updateFunc;
	UpdateFunc_t m_tutorialUpdateFunc;
	DrawFunc_t m_drawFunc;

	std::shared_ptr<GateManager> m_gateMgr;
	std::shared_ptr<StageManager> m_stageMgr;
	std::shared_ptr<CameraManager> m_cameraMgr;

	std::shared_ptr<Player> m_player;

	std::vector<int> m_rtTable;
	std::vector<TutorialInfo> m_tutorialInfo;

	CBuff* m_cBuff;

	int m_cBuffH;
	int m_time;
	int m_index;
	int m_checkFrame;
	int m_stampFrame;
	int m_uiFrame1;
	int m_uiFrame2;

	bool m_isDrawGoal;
	bool m_isDrawClear;
};
