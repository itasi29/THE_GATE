#pragma once
#include "FileBase.h"

/// <summary>
/// シェーダーファイルを管理するクラス
/// </summary>
class FileShader : public FileBase
{
public:
	FileShader(FileManager& mgr);
	virtual ~FileShader();

private:
	/// <summary>
	/// ハンドルを削除
	/// </summary>
	void Delete() override;
};

