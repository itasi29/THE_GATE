#pragma once
#include "SceneBase.h"
#include <vector>
#include <list>
#include <memory>

class GateManager;
class StageManager;
class CameraManager;
class Player;
class UIMoveData;

class SceneMain final : public SceneBase
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
	SceneMain(const wchar_t* const stageName);
	~SceneMain() {}

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

	/// <summary>
	/// 再スタート
	/// </summary>
	void OnRestart();

private:
	void MainUpdate();
	void GameOverUpdate();
	void RestartUpdate();

	void DrawMain() const;
	void DrawRestart() const;

	void DrawUI() const;
	void DrawCommonUI() const;
	void DrawExistUI() const;
	void DrawDeathUI() const;

private:
	using UpdateFunc_t = void(SceneMain::*)();
	using DrawFunc_t = void(SceneMain::*)() const;
	UpdateFunc_t m_updateFunc;
	DrawFunc_t m_drawFunc;

	std::shared_ptr<GateManager> m_gateMgr;
	std::shared_ptr<StageManager> m_stageMgr;
	std::shared_ptr<CameraManager> m_cameraMgr;

	std::shared_ptr<Player> m_player;

	std::shared_ptr<UIMoveData> m_deathUIWindow;
	std::vector<std::shared_ptr<UIMoveData>> m_deathUIList;

	std::vector<int> m_rtTable;

	const wchar_t* const m_stageName;

	CBuff* m_cBuff;

	int m_cBuffH;
	int m_time;
	int m_current;
	int m_count;
};
