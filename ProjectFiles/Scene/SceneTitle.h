#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class GateManager;
class StageManager;
class CameraManager;
class UIMoveData;
class Player;
class Camera;
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
	};

	struct CBuff
	{
		float frame;
		float fireRed;
		float fireGreen;
		float fireBlue;
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
	/// セーブデータ情報描画
	/// </summary>
	void DrawSaveInfo(int saveNo) const;
	/// <summary>
	/// PadUIの描画
	/// </summary>
	void DrawPadUI() const;

	/// <summary>
	/// ゲームの開始
	/// </summary>
	void OnStart();

private:
	using UpdateFunc_t = void(SceneTitle::*)();
	UpdateFunc_t m_updateFunc;
	using DrawFunc_t = void(SceneTitle::*)() const;
	DrawFunc_t m_drawFunc;

	std::vector<std::shared_ptr<StageManager>> m_stageMgrs;
	std::shared_ptr<CameraManager> m_cameraMgr;
	std::shared_ptr<Camera> m_nowCamera;
	std::shared_ptr<Camera> m_nextCamera;

	State m_state;

	std::shared_ptr<UIMoveData> m_mainUI;
	std::shared_ptr<UIMoveData> m_saveUI;
	std::shared_ptr<UIMoveData> m_saveInfoUI;
	std::shared_ptr<UIMoveData> m_decideUI;
	std::shared_ptr<UIMoveData> m_rankingUI;

	CBuff* m_cBuff;

	// 定数バッファハンドル
	int m_cBuffH;
	// カメラ描画先RT
	int m_cameraRt1;
	int m_cameraRt2;
	// 選択カーソル
	int m_menuCurrent;		// メニュー
	int m_saveDataNo;		// セーブデータ番号
	int m_decideCurrent;	// 続きから・初めから
	int m_rankingCurrent;	// ランキング番号
	// フレーム系
	int m_fadeSaveInfoFrame;	// セーブ情報関係のフェードフレーム
	int m_fadeSaveNoFrame;		// セーブデータ番号変更フェード
	int m_uiCount;
	// ステージ系
	int m_cameraFrame;
	int m_stageChangeFrame;
	int m_stageIndex;

	float m_stageFade;			// ステージ変更

	// フェードフラグ
	bool m_isFadeSaveNo;		// セーブデータ番号変更
	// 描画フラグ
	bool m_isNowSelectMenu;		// 現在選択がメニュー
	bool m_isNowSelectSaveData;	// 現在選択がセーブデータ番号選択
	bool m_isStageChangeFade;	// ステージ変更フェード
};

