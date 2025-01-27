#pragma once
#include <unordered_map>
#include <vector>
#include <string>

/// <summary>
/// セーブデータを格納する構造体
/// </summary>
struct SaveData
{
    bool isExist = false;                            // セーブデータが存在するか
    int totalTime = 0;                               // 総プレイ時間
	int cameraSens = 0; 						     // カメラ感度
    std::unordered_map<std::wstring, bool> isClear;  // ステージクリア状況
    std::unordered_map<std::wstring, int> clearTime; // ステージクリア時間
};

using SaveDataTable_t = std::vector<SaveData>;

/// <summary>
/// セーブデータを管理するクラス
/// </summary>
class SaveDataManager final
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SaveDataManager();

	SaveDataManager(const SaveDataManager&) = delete;
	SaveDataManager(const SaveDataManager&&) = delete;
	SaveDataManager& operator= (const SaveDataManager&) = delete;
	SaveDataManager& operator= (const SaveDataManager&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>SaveDataManagerのインスタンス</returns>
    static SaveDataManager& GetInstance();

    /// <summary>
    /// セーブデータの読み込み
    /// </summary>
    void Load();
    /// <summary>
    /// セーブデータの保存
    /// </summary>
    void Save() const;

    /// <summary>
    /// セーブデータの初期化
    /// </summary>
    /// <param name="no">セーブデータ番号</param>
    void Initialized(int no);

    /// <summary>
    /// 使用するセーブデータの決定
    /// </summary>
    /// <param name="no">セーブデータ番号</param>
    void DecideUseSaveNo(int no) { m_useSaveNo = no; }

    /// <summary>
    /// 総プレイ時間の更新
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    void UpdateTime(const wchar_t* const stageName);
    /// <summary>
	/// BGMの音量を変更
    /// </summary>
    /// <param name="changeSize">変更サイズ</param>
    void ChangeBgmRate(int changeSize);
	/// <summary>
	/// SEの音量を変更
	/// </summary>
	/// <param name="changeSize">変更サイズ</param>
	void ChangeSeRate(int changeSize);
	/// <summary>
	/// カメラの感度を変更
	/// </summary>
	/// <param name="changeSize">変更サイズ</param>
    void ChangeCameraSens(int changeSize);

    /// <summary>
    /// ステージをクリアしたことにする
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    /// <param name="clearTime">クリアタイム</param>
    void OnClear(const wchar_t* const stageName, int clearTime);

	/// <summary>
	/// BGMの割合を取得
	/// </summary>
	/// <returns>BGMの割合</returns>
    float GetBgmRate() const { return m_bgmVolume * 0.01f; }
	/// <summary>
	/// SEの割合を取得
	/// </summary>
	/// <returns>SEの割合</returns>
	float GetSeRate() const { return m_seVolume * 0.01f; }
	/// <summary>
	/// BGMの音量を取得
	/// </summary>
	/// <returns>BGMの音量</returns>
	int GetBgmVolume() const { return m_bgmVolume; }
	/// <summary>
	/// SEの音量を取得
	/// </summary>
	/// <returns>SEの音量</returns>
	int GetSeVolume() const { return m_seVolume; }
	/// <summary>
	/// カメラの感度を取得
	/// </summary>
	/// <returns>カメラの感度</returns>
	int GetCameraSens() const { return m_dataTable.at(m_useSaveNo).cameraSens; }

    /// <summary>
    /// セーブデータの数を取得する
    /// </summary>
    /// <returns>セーブデータ数</returns>
    int GetSaveDataNum() const;
    /// <summary>
    /// クリアステージ数を取得する
    /// </summary>
    /// <param name="saveNo">セーブデータ番号</param>
    /// <returns>クリアステージ数</returns>
    int GetClearStageNum(int saveNo) const;
    /// <summary>
    /// そのセーブデータのゲームプレイ時間を取得する
    /// </summary>
    /// <param name="saveNo">セーブデータ番号</param>
    /// <returns>プレイ時間</returns>
    int GetTotalTime(int saveNo) const { return m_dataTable.at(saveNo).totalTime; }
    /// <summary>
    /// 現在使用しているセーブデータ番号を取得する
    /// </summary>
    /// <returns>セーブデータ番号</returns>
    int GetUseSaveNo() const { return m_useSaveNo; }
    /// <summary>
    /// ステージのクリア時間を取得
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    /// <returns>クリア時間</returns>
    int GetStageClearTime(const wchar_t* const stageName) const { return m_dataTable.at(m_useSaveNo).clearTime.at(stageName); }
    /// <summary>
    /// セーブデータが存在するか
    /// </summary>
    /// <param name="no">セーブデータ番号</param>
    /// <returns>true: 存在する/ false: 存在しない</returns>
    bool IsExist(int no) { return m_dataTable.at(no).isExist; }
    /// <summary>
    /// ステージをクリアしているか
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    /// <returns>true: クリアしている/ false: クリアしていない</returns>
    bool IsClear(const wchar_t* const stageName) const { return m_dataTable.at(m_useSaveNo).isClear.at(stageName); }
    /// <summary>
    /// ステージを解放しているか
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    /// <returns>true: 解放している/ false: 解放していない</returns>
    bool IsReleaseStage(const wchar_t* const stageName) const;

private:
    /// <summary>
    /// ファイルからデータを読み込む
    /// </summary>
    /// <param name="handle">ファイルハンドル</param>
    void LoadFile(int handle);

private:
    // 使用するセーブデータ番号
    int m_useSaveNo;  
    // セーブデータのテーブル
    SaveDataTable_t m_dataTable; 
    int m_bgmVolume;
	int m_seVolume;
};
