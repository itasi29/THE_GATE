#include "SaveDataManager.h"
#include <DxLib.h>
#include <cassert>
#include "StringUtility.h"
#include "StageDataManager.h"
#include "SoundManager.h"

namespace
{
	// セーブデータの数
	constexpr int SAVE_DATA_NUM = 3;

	// セーブデータのパス
	const char* const FILE_PATH = "Data/SaveData/SaveData.dat";

	
	constexpr int MAX_BGM_RATE = 100;	// 最大BGM割合
	constexpr int MAX_SE_RATE = 100;	// 最大SE割合
	constexpr int MAX_CAMERA_SENS = 100;	// 最大カメラ感度
}

SaveDataManager::SaveDataManager() :
	m_useSaveNo(0),
	m_bgmVolume(MAX_BGM_RATE),
	m_seVolume(MAX_SE_RATE)
{
}

SaveDataManager& SaveDataManager::GetInstance()
{
	static SaveDataManager instance;
	return instance;
}

void SaveDataManager::Load()
{
	SetUseASyncLoadFlag(false);
	auto path = StringUtility::StringToWString(FILE_PATH);
	int handle = FileRead_open(path.c_str());

	// ファイルが存在しない場合はデフォルトのデータにする
	if (handle == 0)
	{
		m_dataTable.resize(SAVE_DATA_NUM);
		for (int i = 0; i < SAVE_DATA_NUM; ++i)
		{
			Initialized(i);
			m_dataTable.at(i).cameraSens = static_cast<int>(MAX_CAMERA_SENS * 0.5f);
		}
	}
	// ファイルが存在する場合はファイルからデータを読み込む
	else
	{
		LoadFile(handle);
		// ファイルを閉じる
		FileRead_close(handle);
	}
	SetUseASyncLoadFlag(true);
}

void SaveDataManager::Save() const
{
	// ファイルオープン
	FILE* fp;
	auto err = fopen_s(&fp, FILE_PATH, "wb");
	if (err != 0)
	{
		assert(false && "セーブファイルを開くのに失敗しました");
		return;
	}

	// BGMの音量を書き込み
	fwrite(&m_bgmVolume, sizeof(int), 1, fp);
	// SEの音量を書き込み
	fwrite(&m_seVolume, sizeof(int), 1, fp);
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageNum = stageDataMgr.GetStageNum();
	for (const auto& data : m_dataTable)
	{
		// カメラの感度を書き込み
		fwrite(&data.cameraSens, sizeof(int), 1, fp);
		// 存在フラグの書き込み
		fwrite(&data.isExist, sizeof(bool), 1, fp);
		// 総プレイ時間の書き込み
		fwrite(&data.totalTime, sizeof(int), 1, fp);
		// クリア情報の書き込み
		for (int i = 0; i < stageNum; ++i)
		{
			const auto& name = stageDataMgr.GetStageName(i);
			// クリア情報の書き込み
			fwrite(&data.isClear.at(name), sizeof(bool), 1, fp);
			// ステージクリアタイムを書き込み
			fwrite(&data.clearTime.at(name), sizeof(int), 1, fp);
		}
	}

	// ファイルクローズ
	fclose(fp);
}

void SaveDataManager::Initialized(int no)
{
	auto& data = m_dataTable[no];
	// セーブデータを存在しないに
	data.isExist = false;
	// プレイ時間の初期化
	data.totalTime = 0;
	// ステージ数分繰り返す
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageNum = stageDataMgr.GetStageNum();
	for (int i = 0; i < stageNum; ++i)
	{
		// ステージ名取得
		const auto& name = stageDataMgr.GetStageName(i);
		// クリア情報の初期化
		data.isClear[name] = false;
		data.clearTime[name] = 99 * 3600 + 59 * 60 + 59;
	}
}

void SaveDataManager::UpdateTime(const wchar_t* const stageName)
{
	auto& data = m_dataTable.at(m_useSaveNo);
	// データが存在することに
	data.isExist = true;
	// トータルタイムの更新
	++data.totalTime;
}

void SaveDataManager::ChangeBgmRate(int changeSize)
{
	m_bgmVolume = std::min<int>(std::max<int>(m_bgmVolume + changeSize, 0), MAX_BGM_RATE);
	SoundManager::GetInstance().ChangePlayBgmVolume();
}

void SaveDataManager::ChangeSeRate(int changeSize)
{
	m_seVolume = std::min<int>(std::max<int>(m_seVolume + changeSize, 0), MAX_SE_RATE);
}

void SaveDataManager::ChangeCameraSens(int changeSize)
{
	m_dataTable.at(m_useSaveNo).cameraSens = std::min<int>(std::max<int>(m_dataTable.at(m_useSaveNo).cameraSens + changeSize, 0), 100);
}

void SaveDataManager::OnClear(const wchar_t* const stageName, int clearTime)
{
	auto& data = m_dataTable.at(m_useSaveNo);

	// クリア時間が保存されているプレイ時間を超えている場合は更新
	if (data.clearTime.at(stageName) > clearTime) data.clearTime.at(stageName) = clearTime;
	// クリアしたことに
	data.isClear.at(stageName) = true;
}

int SaveDataManager::GetSaveDataNum() const
{
	return SAVE_DATA_NUM;
}

int SaveDataManager::GetClearStageNum(int saveNo) const
{
	bool isFirst = true;
	int count = 0;
	for (auto& isClear : m_dataTable.at(saveNo).isClear)
	{
		// 最初はタイトルデータなので飛ばす
		if (isFirst)
		{
			isFirst = false;
			continue;
		}
		// クリアしていなければ抜ける
		if (!isClear.second) break;
		// カウント増加
		++count;
	}
	return count;
}

bool SaveDataManager::IsReleaseStage(const wchar_t* const stageName) const
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	// ステージ番号取得
	int no = stageDataMgr.GetStageNo(stageName);
	// ステージが1以下なら解放していることに(0番はタイトルの情報)
	if (no <= 1) return true;
	// それ以外は一つ前のステージがクリアしていたら解放していることにする
	const auto& name = stageDataMgr.GetStageName(no - 1);
	return m_dataTable.at(m_useSaveNo).isClear.at(name);
}

void SaveDataManager::LoadFile(int handle)
{
	auto& stageDataMgr = StageDataManager::GetInstance();

	// BGMの音量を読み込み
	FileRead_read(&m_bgmVolume, sizeof(int), handle);
	// SEの音量を読み込み
	FileRead_read(&m_seVolume, sizeof(int), handle);

	// ステージ数取得
	const auto& stageNum = stageDataMgr.GetStageNum();
	for (int i = 0; i < SAVE_DATA_NUM; ++i)
	{
		SaveData data;
		// カメラの感度を読み込み
		FileRead_read(&data.cameraSens, sizeof(int), handle);
		// 存在フラグを読み込み
		FileRead_read(&data.isExist, sizeof(bool), handle);
		// プレイ時間の読み込み
		FileRead_read(&data.totalTime, sizeof(int), handle);
		for (int j = 0; j < stageNum; ++j)
		{
			// ステージ名取得
			const auto& name = stageDataMgr.GetStageName(j);
			// クリア情報読み込み
			bool isClear;
			FileRead_read(&isClear, sizeof(bool), handle);
			data.isClear[name] = isClear;
			// ステージクリアタイム読み込み
			FileRead_read(&data.clearTime[name], sizeof(int), handle);
		}

		m_dataTable.emplace_back(data);
	}
}
