#pragma once
#include "SceneBase.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <Input.h>

class UIMoveData;
class SceneBase;

class SceneOption : public SceneBase
{
public:
	SceneOption();
	~SceneOption() {}

	// 初期化処理(非同期)
	void AsyncInit() override;
	// 初期化処理(同期)
	void Init() override;
	void Init(SceneBase* scene) { m_scene = scene; }
	// 終了処理
	void End() override;
	// 更新処理(同期)
	void Update(bool isFade) override;
	// 描画処理
	void Draw() const override;

	void CheckOpenScene(SceneKind openScene);

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void MainTitleUpdate();
	void MainStageSelectUpdate();
	void MainMainUpdate();

	void SoundUpdate();
	void CameraUpdate();
	void RestartUpdate();
	void ReturnStageSelectUpdate();

	void ChangeSubDraw();

	void OnMenu();
	void OnFadeOut();
	void OnSound();
	void OnCamera();
	void OnRestart();
	void OnReturnStageSelect();

	void DrawFade() const;
	void DrawCommon() const;
	void DrawSound() const;
	void DrawCamera() const;
	void DrawRestart() const;
	void DrawReturnStageSelect() const;
	void DrawClose() const;

	void DrawSoundBar(int y, int volume) const;
	void DrawSubFrame() const;
	void DrawPadUI() const;

private:
	using UpdateFunc_t = void(SceneOption::*)();
	UpdateFunc_t m_updateFunc;
	using DrawFunc_t = void(SceneOption::*)() const;
	DrawFunc_t m_drawFunc;
	DrawFunc_t m_subDrawFunc;

	SceneBase* m_scene;

	std::unordered_map<SceneKind, std::vector<std::wstring>> m_selectStrList;

	std::shared_ptr<UIMoveData> m_mainUI;
	std::shared_ptr<UIMoveData> m_subUI;

	SceneKind m_openScene;
	RepeatKind m_preRepeatKind;

	int m_mainCurrent;
	int m_subCurrent;
	int m_uiCount;

	float m_fadeCount;

	bool m_isNowMenu;
};
