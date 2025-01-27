#pragma once
#include "FileBase.h"

/// <summary>
/// サウンドファイルを管理するクラス
/// </summary>
class FileSound : public FileBase
{
public:
	FileSound(FileManager& mgr);
	virtual ~FileSound();

private:
	/// <summary>
	/// ハンドルを削除
	/// </summary>
	void Delete() override;
};

