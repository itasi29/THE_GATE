﻿#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class GateManager;
class StageManager;
class CameraManager;
class UIMoveData;
class Player;
enum class CameraKind;

class SceneTitle : public SceneBase
{
private:
	enum class State
	{
		MENU,
		RANKING,
		SELECT_SAVE_DATA,
		DECIDE_SAVE_DATA,
		CHECK_DELETE,
	};

public:
	SceneTitle();
	~SceneTitle() {}

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

private:
	/// <summary>
	/// ステージ情報の更新
	/// </summary>
	void StageUpdate();
	/// <summary>
	/// メニュー(セーブデータ・オプション・終了)の選択
	/// </summary>
	void SelectMenuUpdate();
	/// <summary>
	/// ランキング
	/// </summary>
	void RankingUpdate();
	/// <summary>
	/// どのセーブデータを使うかの選択
	/// </summary>
	void SelectSaveDataUpdate();
	/// <summary>
	/// そのセーブデータで始める・消すの選択
	/// </summary>
	void DecideSaveDataUpdate();
	/// <summary>
	/// 本当に消すかの選択
	/// </summary>
	void CheckDeleteSaveDataUpdate();

	/// <summary>
	/// メニュー選択
	/// </summary>
	void OnSelectMenu();
	/// <summary>
	/// ランキング
	/// </summary>
	void OnRanking();
	/// <summary>
	/// セーブデータ選択
	/// </summary>
	void OnSelectSaveData();
	/// <summary>
	/// セーブデータ決定
	/// </summary>
	void OnDecideSaveData();
	/// <summary>
	/// 削除確認
	/// </summary>
	void OnCheckDeleteSaveData();
	
	/// <summary>
	/// ステージの描画
	/// </summary>
	void DrawStage() const;
	void DrawNormal() const;
	void DrawBlend() const;
	void DrawGateBlend(int rt, CameraKind gate, CameraKind from) const;
	void DrawModelBlend(int rt, int tex1, int tex2, CameraKind camera, bool isPlayerDraw) const;

	/// <summary>
	/// メニュー選択時の描画
	/// </summary>
	void DrawSelectMenu() const;
	/// <summary>
	/// ランキングの描画
	/// </summary>
	void DrawRanking() const;
	/// <summary>
	/// セーブデータ選択時の描画
	/// </summary>
	void DrawSelectSaveData() const;
	/// <summary>
	/// 続きから・初めから選択時の描画
	/// </summary>
	void DrawDecideSaveData() const;
	/// <summary>
	/// 削除確認時の描画
	/// </summary>
	void DrawCheckDeleteSaveData() const;
	/// <summary>
	/// セーブデータ情報描画
	/// </summary>
	void DrawSaveInfo(int saveNo) const;
	/// <summary>
	/// PadUIの描画
	/// </summary>
	void DrawPadUI() const;

	/// <summary>
	/// カーソルの更新
	/// </summary>
	/// <param name="current">カーソル</param>
	/// <param name="list">UI移動データ</param>
	/// <param name="start">スタート位置</param>
	/// <param name="interval">動かす大きさ</param>
	/// <param name="upCmd">コマンド(ー方向)</param>
	/// <param name="downCmd">コマンド(＋方向)</param>
	bool CurrentUpdate(int& current, const std::vector<std::shared_ptr<UIMoveData>>& list, int start, int interval, const char* const upCmd, const char* const downCmd);

	/// <summary>
	/// ゲームの開始
	/// </summary>
	void OnStart();

	void ArmWindowDraw(const std::shared_ptr<UIMoveData>& data, int subArmFrameX, int subArmFrameY, int subArmX, int subArmY, int windowFileName, int armFrameFileName, int armFileName) const;

private:
	using UpdateFunc_t = void(SceneTitle::*)();
	UpdateFunc_t m_updateFunc;
	using DrawFunc_t = void(SceneTitle::*)() const;
	DrawFunc_t m_drawFunc;

	std::shared_ptr<StageManager> m_stageMgr;
	std::shared_ptr<CameraManager> m_cameraMgr;
	std::shared_ptr<GateManager> m_gateMgr;
	std::shared_ptr<Player> m_player;

	State m_state;

	std::vector<std::shared_ptr<UIMoveData>> m_menuUIList;
	std::vector<std::shared_ptr<UIMoveData>> m_saveUIList;
	std::vector<std::shared_ptr<UIMoveData>> m_decideUIList;
	std::vector<std::shared_ptr<UIMoveData>> m_checkUIList;
	std::shared_ptr<UIMoveData> m_saveInfoUI;
	std::shared_ptr<UIMoveData> m_saveArmWindowUI;
	std::shared_ptr<UIMoveData> m_decideArmWindowUI;
	std::shared_ptr<UIMoveData> m_alertUI;
	std::shared_ptr<UIMoveData> m_rankingUI;

	std::vector<int> m_rtTable;

	int m_waitShotFrame;
	int m_menuCurrent;
	int m_saveDataNo;
	int m_decideCurrent;
	int m_checkCurrent;
	int m_rankingCurrent;
	int m_preSaveDataNo;
	int m_fadeFrame;
	int m_waveCount;

	bool m_isShot;
	bool m_isFade;
	bool m_isFadeTime;
	bool m_isDrawMenu;
	bool m_isDrawSelectSave;
};

