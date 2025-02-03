#pragma once
#include "SceneBase.h"

enum class RankKind;

/// <summary>
/// ステージ選択シーンクラス
/// </summary>
class SceneStageSelect : public SceneBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneStageSelect();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneStageSelect() {}

    /// <summary>
    /// 初期化処理(非同期)
    /// </summary>
    void AsyncInit() override;

    /// <summary>
    /// 初期化処理(同期)
    /// </summary>
    void Init() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理(同期)
    /// </summary>
    /// <param name="isFade">フェード中かどうか</param>
    void Update(bool isFade) override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const override;

private:
    /// <summary>
    /// ステージ選択の更新処理
    /// </summary>
    void SelectUpdate();

    /// <summary>
    /// ステージフレームの描画処理
    /// </summary>
    /// <param name="stageNum">ステージ番号</param>
    void DrawStageFrame(int stageNum) const;

    /// <summary>
    /// ランクタイムの描画処理
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    void DrawRankTime(const wchar_t* const stageName) const;

    /// <summary>
    /// ランクタイムの描画処理
    /// </summary>
    /// <param name="y">Y座標</param>
    /// <param name="graphX">グラフのX座標</param>
    /// <param name="strX">文字列のX座標</param>
    /// <param name="stageName">ステージ名</param>
    /// <param name="rank">ランクの種類</param>
    /// <param name="rankId">ランクID</param>
    void DrawRankTime(int y, int graphX, int strX, const wchar_t* const stageName, RankKind rank, int rankId) const;

    /// <summary>
    /// PadUIの描画処理
    /// </summary>
    void DrawPadUI() const;

private:
    // 現在選択されているステージ
    int m_selectCurrent; 
    // カウント
    int m_count; 
};