#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include "StringUtility.h"
#include "SoundManager.h"
#include "SaveDataManager.h"
#include "Collidable.h"

namespace
{
	// ボリューム
	constexpr int VOLUME_VAL = 255;

	// 音量を減少させる距離
	constexpr float SOUND_DEL_DIS = 50.0f;
	constexpr float SOUND_DEL_SQ_DIS = SOUND_DEL_DIS * SOUND_DEL_DIS;

	// データ保存パス
	const char* const FILE_PATH = "Data/SaveData/SoundData.dat";
}

SoundManager::SoundManager() :
	m_nowPlayBgm(-1),
	m_soundHandle(-1),
	m_soundSavePos(0),
	m_isStop(false)
{
}

SoundManager::~SoundManager()
{
}

SoundManager& SoundManager::GetInstance()
{
	static SoundManager snd;
	return snd;
}

void SoundManager::Update()
{
	if (m_isStop) return;

	// Seの中心とするものがある場合
	if (!m_seCenter.expired())
	{
		const auto& center = m_seCenter.lock()->GetPos();

		for (auto& info : m_saveSeList)
		{
			// オブジェクトの距離によって音を変更しない場合次の処理へ
			if (info.master.expired()) continue;
			// サウンドが終了していれば次の処理へ
			if (!CheckSoundMem(info.handle)) continue;

			const auto& target = info.master.lock()->GetPos();
			float rate = 1.0f - std::min<float>((target - center).SqLength() / SOUND_DEL_SQ_DIS, 1.0f);
			ChangePlayVolume(info.handle, rate, false);
		}
	}
	
	m_saveSeList.remove_if(
		[](const auto& info)
		{
			return !CheckSoundMem(info.handle);
		});
}

void SoundManager::StopNowPlaySe()
{
	m_isStop = true;

	for (auto& info : m_saveSeList)
	{
		info.savePos = GetSoundCurrentPosition(info.handle);
		StopSoundMem(info.handle);
	}
}

void SoundManager::RestartStopedSe()
{
	m_isStop = false;

	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetSeRate() * 0.01f);
	for (auto& info : m_saveSeList)
	{
		SetSoundCurrentTime(info.savePos, info.handle);
		ChangeNextPlayVolumeSoundMem(volume, info.handle);
		PlaySoundMem(info.handle, DX_PLAYTYPE_BACK, false);
	}
}

void SoundManager::PlayBgm(int soundHandle, bool isLoop, bool isSoundPosSave)
{
	if (soundHandle < 0)
	{
		soundHandle = m_nowPlayBgm;
	}

	if (IsNowPlay(soundHandle)) return;

	// 音量の変更
	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetBgmRate());
	ChangeNextPlayVolumeSoundMem(volume, soundHandle);

	if (isSoundPosSave)
	{
		SetSoundCurrentTime(m_soundSavePos, soundHandle);
		PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK, isLoop);
	}
	else
	{
		PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK, isLoop);
	}

	m_nowPlayBgm = soundHandle;
}

void SoundManager::PlayFadeBgm(int soundHandle, float rate, bool isLoop)
{
	if (soundHandle < 0)
	{
		soundHandle = m_nowPlayBgm;
	}

	if (IsNowPlay(soundHandle))
	{
		ChangePlayVolume(m_nowPlayBgm, 1.0f, true);
		return;
	}

	// 音量の変更
	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetBgmRate() * rate);
	ChangeNextPlayVolumeSoundMem(volume, soundHandle);

	// ループがONの場合はBGMが終了次第先頭に戻る
	PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK, isLoop);

	m_nowPlayBgm = soundHandle;
}

void SoundManager::PlaySe(int seHandle, bool isOption)
{
	// 音量の変更
	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetBgmRate());
	ChangeNextPlayVolumeSoundMem(volume, seHandle);

	auto err = PlaySoundMem(seHandle, DX_PLAYTYPE_BACK, true);
	assert(err != -1);
	m_soundHandle = seHandle;

	if (!isOption)
	{
		m_saveSeList.emplace_back(SaveSeData{ seHandle, 0 });
	}
}

void SoundManager::PlaySe3D(int seHandle, const std::weak_ptr<MyEngine::Collidable> master, bool isOption)
{
	// 音量の変更
	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetBgmRate());
	ChangeNextPlayVolumeSoundMem(volume, seHandle);

	auto err = PlaySoundMem(seHandle, DX_PLAYTYPE_BACK, true);
	assert(err != -1);
	m_soundHandle = seHandle;

	if (!isOption)
	{
		m_saveSeList.emplace_back(SaveSeData{ seHandle, 0, master });
	}
}

void SoundManager::StopBGM(int soundHandle, bool isSoundPosSave)
{
	if (soundHandle < 0)
	{
		soundHandle = m_nowPlayBgm;
	}

	StopSoundMem(soundHandle);

	// 音声再生場所の保存
	if (isSoundPosSave)
	{
		m_soundSavePos = GetSoundCurrentPosition(soundHandle);
	}
}

void SoundManager::Stop(int handle)
{
	StopSoundMem(handle);
}

void SoundManager::ChangePlayBgmVolume()
{
	ChangePlayVolume(m_nowPlayBgm, 1.0f, true);
}

bool SoundManager::IsNowPlay(int handle) const
{
	auto err = CheckSoundMem(handle);
	assert(err != -1);
	return err;
}

void SoundManager::ChangePlayVolume(int handle, float rate, bool isBgm)
{
	// 再生中でなければ終了
	if (!IsNowPlay(handle)) return;

	// ズレていたらより正確なこっちで GetSoundCurrentTime 
	LONGLONG soundPosition = GetSoundCurrentPosition(handle);

	// 一度サウンドを止める
	StopSoundMem(handle);

	// 再生位置を設定
	SetSoundCurrentTime(soundPosition, handle);

	// 音量の調整
	auto& saveDataMgr = SaveDataManager::GetInstance();
	int volume;
	if (isBgm)	volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetBgmRate() * rate);
	else		volume = static_cast<int>(VOLUME_VAL * saveDataMgr.GetSeRate() * rate);
	ChangeNextPlayVolumeSoundMem(volume, handle);

	// 再度再生
	PlaySoundMem(handle, DX_PLAYTYPE_BACK, false);
}
