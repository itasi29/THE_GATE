#pragma once
#include <cstdio>

class Application final
{
private:
	Application();
	
	Application(const Application& app) = delete;
	void operator= (const Application& app) = delete;

public:
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
	void InitManager();
	void EndManager();
#ifdef _DEBUG
	void ShowTime() const;
#endif

private:
	bool m_isEnd;
	bool m_isWindows;

#ifdef _DEBUG
	FILE* m_in;
	FILE* m_out;
#endif
};

