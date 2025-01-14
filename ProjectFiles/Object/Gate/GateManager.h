#pragma once
#include <memory>
#include <list>
#include <unordered_map>

namespace MyEngine
{
	struct CollideHitInfo;
	class Collidable;
	class ColliderCapsule;
	struct PreHitInfo;
}
class Vec3;
class Gate;
class GateBullet;
class CameraManager;
class Player;
enum class ObjectTag;

enum class GateKind
{
	Orange,
	Blue,
	MAX
};

class GateManager
{
public:
	GateManager(const std::shared_ptr<CameraManager>& cameraMgr, const wchar_t* const stageName);
	~GateManager();

	void Init(const std::weak_ptr<Player>& player);
	void End();
	void Update();
	void DrawGate(int texA, int texB) const;

	void Restart();

	void AddBullet(std::shared_ptr<GateBullet> bullet);
	void CreateGate(GateKind kind, MyEngine::Collidable* collider, const MyEngine::CollideHitInfo& hitInfo, const Vec3& dir);
	
	std::shared_ptr<Gate> GetPairGate(GateKind kind) const;
	const GateKind& GetLastShotKind() const { return m_lastShotKind; }
	bool IsCreateBothGates() const { return m_isCreate.at(GateKind::Blue) && m_isCreate.at(GateKind::Orange); }
	bool IsCreate(GateKind kind) const { return m_isCreate.at(kind); }

private:
	void CreateGateOnTerrain();
	void CreateGate(std::shared_ptr<Gate>& gate, GateKind kind, ObjectTag hitTag, const Vec3& pos, const Vec3& norm, const Vec3& dir);

private:
	std::shared_ptr<CameraManager> m_cameraMgr;
	std::shared_ptr<Gate> m_gateOrange;
	std::shared_ptr<Gate> m_gateBlue;
	std::list<std::shared_ptr<GateBullet>> m_bulletList;
	std::weak_ptr<Player> m_player;

	const wchar_t* const m_stageName;

	std::unordered_map<GateKind, bool> m_isCreate;

	GateKind m_lastShotKind;
};
