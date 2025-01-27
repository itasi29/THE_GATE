#pragma once
#include "FileBase.h"

/// <summary>
/// Effekseerファイルを管理するクラス
/// </summary>
class FileEffekseer : public FileBase
{
public:
	FileEffekseer(FileManager& mgr);
	virtual ~FileEffekseer();

private:
	/// <summary>
	/// ハンドルの削除
	/// </summary>
	void Delete() override;
};

