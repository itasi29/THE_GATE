#pragma once
#include <memory>
#include <unordered_map>

class SceneBase;
class SceneMain;
class FileBase;
enum class SceneKind;

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
private:
	SceneManager();

	SceneManager(const SceneManager& mgr) = delete;
	void operator= (const SceneManager& mgr) = delete;

public:
	virtual ~SceneManager();

	static SceneManager& GetInstance();

	void Init();
	void Update();
	void Draw() const;
	void End();

	/// <summary>
	/// シーンの変更
	/// </summary>
	void Change(const std::shared_ptr<SceneBase>& next);

	/// <summary>
	/// 流すBgmを変更する
	/// </summary>
	/// <param name="handle">流すBgmのハンドル</param>
	void ChangeBgmH(int handle);

	/// <summary>
	/// オプションを開く
	/// </summary>
	/// <param name="openScene">開いたシーンの種類</param>
	/// <param name="sceneMain">リスタート用のメインシーンを渡す用</param>
	void OpenOption(SceneKind openScene, const SceneMain* sceneMain = nullptr);
	/// <summary>
	/// オプションorポーズを閉じる
	/// </summary>
	void CloseOption();

	/// <summary>
	/// オプションを開いているか
	/// </summary>
	/// <returns>true: 開いている /false: 開いていない</returns>
	bool IsOpenOption() const { return m_option != nullptr; }
	/// <summary>
	/// フェードが終了したフレーム化
	/// </summary>
	/// <returns>true: 終了したフレーム /false: それ以外のフレーム</returns>
	bool IsFadeEndFrame() const { return m_isFadeEndFrame; }
	/// <summary>
	/// 現在のシーンの種類を取得
	/// </summary>
	/// <returns>シーンの種類</returns>
	SceneKind GetNowSceneKind() const;

private:
	using UpdateFunc_t = void(SceneManager::*)();
	void NormalUpdate();
	void FadeOut();
	void FileLoadingUpdate();
	void FadeIn();

	using DrawFunc_t = void(SceneManager::*)() const;
	void DrawNormal() const;
	void DrawFade() const;
	void FileLoadingDraw() const;

private:
	std::shared_ptr<SceneBase> m_option;
	std::shared_ptr<SceneBase> m_scene;
	std::shared_ptr<SceneBase> m_nextScene;

	std::unordered_map<int, std::shared_ptr<FileBase>> m_files;

	UpdateFunc_t m_updateFunc;
	DrawFunc_t m_drawFunc;

	// 流しているBGMのハンドル
	int m_bgmH;

	// フェードアウトに使うもの
	float m_count;
	float m_angleSpeed;

	bool m_isFadeEndFrame;
};

