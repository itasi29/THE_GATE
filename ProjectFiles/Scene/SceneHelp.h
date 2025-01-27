#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "CameraManager.h"

class GateManager;
class StageManager;
class Player;
enum class CameraKind;

class SceneHelp
{
private:
	SceneHelp();

	SceneHelp(const SceneHelp& mgr) = delete;
	void operator= (const SceneHelp& mgr) = delete;

public:
	virtual ~SceneHelp();

	static SceneHelp& GetInstance();

	void Init();
	void End();

	void DrawModel(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int drawRt = -1, CameraKind mainCamera = CameraKind::PLAYER, std::function<void()> func = nullptr) const;

private:
	void DrawGateBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, CameraKind gate, CameraKind gateFrom, std::function<void()> func) const;
	void DrawModelBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, int tex1, int tex2, CameraKind kind, bool isPlayerDraw, std::function<void()> func) const;

private:
	std::vector<int> m_rtTable;
};
