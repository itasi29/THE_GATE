#pragma once
#include <cstdio>

/// <summary>
/// アプリケーション全体を管理するクラス
/// </summary>
class Application final
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Application();

    Application(const Application&) = delete;
    Application(const Application&&) = delete;
    Application operator= (const Application&) = delete;
    Application operator= (const Application&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>Applicationのインスタンス</returns>
    static Application& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <returns>true: 成功/ false:失敗</returns>
    bool Init();
    /// <summary>
    /// ゲームの実行
    /// </summary>
    void Run();
    /// <summary>
    /// 終了処理
    /// </summary>
    void Terminate();

    /// <summary>
    /// ゲームを終了させる
    /// </summary>
    void OnEnd() { m_isEnd = true; }

private:
    /// <summary>
    /// マネージャーの初期化
    /// </summary>
    void InitManager();
    /// <summary>
    /// マネージャーの終了処理
    /// </summary>
    void EndManager();
#ifdef _DEBUG
    /// <summary>
    /// デバッグ用の時間表示
    /// </summary>
    void ShowTime() const;
#endif

private:
    // ゲーム終了フラグ
    bool m_isEnd;      
    // FullWindowsフラグ
    bool m_isWindows;  

#ifdef _DEBUG
    FILE* m_in;   // デバッグ用入力ファイル
    FILE* m_out;  // デバッグ用出力ファイル
#endif
};