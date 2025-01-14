#pragma once
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>

class FileBase;

enum class LoadType
{
	IMAGE,
	SOUND,
	MODEL,
	EFFEKSEER,
	PIXEL_SHADER,
	VERTEX_SHADER
};

struct Path
{
	LoadType type = LoadType::IMAGE;
	std::wstring path = L"";
};

struct Shader
{
	int vs;
	int ps;
};

// パスデータ
using PathData_t = std::vector<Path>;
// ロード済みデータ
using LoadedData_t = std::unordered_map<int, std::shared_ptr<FileBase>>;
// モデルの頂点シェーダデータ
using ShaderData_t = std::unordered_map<int, Shader>;
// CSVデータ
using CsvData_t = std::list<std::vector<std::wstring>>;

/// <summary>
/// ファイル管理クラス
/// </summary>
class FileManager
{
private:
	FileManager();

	FileManager(const FileManager& mgr) = delete;
	void operator= (const FileManager& mgr) = delete;

public:
	~FileManager();

	static FileManager& GetInstance();

	// 初期化処理
	void Init();
	// 終了処理
	void End();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	CsvData_t LoadCsv(const wchar_t* const path);

	/// <summary>
	/// データの読み込み
	/// </summary>
	/// <param name="id">読み込みたいデータID</param>
	/// <param name="isEternal">true : 永続 / false : 使っているものが消えたら削除</param>
	/// <returns>データ</returns>
	std::shared_ptr<FileBase> Load(int id, bool isEternal = false);
	/// <summary>
	/// 使用頂点シェーダのハンドル取得
	/// </summary>
	/// <param name="id">モデルID</param>
	/// <returns>頂点シェーダハンドル</returns>
	int GetVS(int id) const { return m_shaderData.at(id).vs; }
	/// <summary>
	/// 使用ピクセルシェーダのハンドル取得
	/// </summary>
	/// <param name="name">モデルID</param>
	/// <returns>ピクセルシェーダハンドル</returns>
	int GetPS(int id) const { return m_shaderData.at(id).ps; }
	/// <summary>
	///	データの削除
	/// </summary>
	/// <param name="id">消したいファイルID</param>
	void Delete(int id);

private:
	/// <summary>
	/// タイプに合わせてクラスを作成
	/// </summary>
	/// <param name="id">ファイルID</param>
	/// <returns>Fileクラス</returns>
	std::shared_ptr<FileBase> MakeFile(int id);
	/// <summary>
	/// タイプに合わせてクラスをキャスト・コピーを返す
	/// </summary>
	/// <param name="file">ベース</param>
	/// <param name="name">名前</param>
	/// <returns>コピー</returns>
	std::shared_ptr<FileBase> CastCopyFile(std::shared_ptr<FileBase>& data);
	/// <summary>
	/// ハンドルの入手
	/// タイプに合わせてロードの仕方を変更
	/// </summary>
	/// <param name="id">ファイルID</param>
	/// <returns>ハンドル</returns>
	int GetHandle(int id) const;

	/// <summary>
	/// パスデータのロード
	/// </summary>
	void LoadPath();
	/// <summary>
	/// モデルシェーダデータのロード
	/// </summary>
	void LoadModelShader();

private:
	bool m_isEnd;

	PathData_t m_path;
	LoadedData_t m_file;
	ShaderData_t m_shaderData;
};

