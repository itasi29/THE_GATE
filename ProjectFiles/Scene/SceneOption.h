#pragma once
#include "SceneBase.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <Input.h>

class UIMoveData;
class SceneMain;

class SceneOption : public SceneBase
{
public:
	SceneOption();
	~SceneOption() {}

	// 初期化処理(非同期)
	void AsyncInit() override;
	// 初期化処理(同期)
	void Init() override;
	void Init(const SceneMain* scene) { m_sceneMain = scene; }
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
	void SelectTitleUpdate();
	void SelectMainUpdate();
	void CheckMainUpdate();

	void SoundChangeUpdate(int current);

	void DrawFade() const;
	void DrawCommon() const;
	void DrawSoundBar(int y, float volumeRate) const;
	void DrawCheck() const;

private:
	using UpdateFunc_t = void(SceneOption::*)();
	UpdateFunc_t m_updateFunc;
	using DrawFunc_t = void(SceneOption::*)() const;
	DrawFunc_t m_drawFunc;

	const SceneMain* m_sceneMain;

	std::unordered_map<SceneKind, std::vector<std::wstring>> m_selectStrList;

	std::vector<std::shared_ptr<UIMoveData>> m_uiList;
	std::vector<std::shared_ptr<UIMoveData>> m_checkUiList;
	std::wstring m_checkStr;

	SceneKind m_openScene;
	RepeatKind m_preKind;

	int m_selectCurrent;
	int m_checkCurrent;
	int m_waveCount;

	float m_fadeCount;
};
