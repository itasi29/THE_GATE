#pragma once
#include "Vec3.h"
#include "Quaternion.h"

struct CameraInfo
{
	// Tpsフラグ
	bool isTps = false;
	// near, far
	float n = 1.0f;
	float f = 100.0f;
	// 視野角
	float fov = 0.0f;
	// ターゲット位置
	Vec3 targetPos;
	// 正面方向
	Vec3 front;
	// 右方向
	Vec3 right;
	// 縦方向の回転具合
	float vertexAngle;
	Quaternion vertexRot;
	// 見ている方向
	Vec3 look;
	// 場所
	Vec3 cameraPos;
};

namespace MyEngine
{
	class ColliderSphere;
	struct CollideHitInfo;
}
struct Quaternion;
class GateManager;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	void Update();
	/// <summary>
	/// 情報の取得
	/// </summary>
	/// <returns>情報群</returns>
	const CameraInfo& GetInfo() const { return m_info; }
	
	/// <summary>
	/// near,farを設定
	/// </summary>
	/// <param name="n">near</param>
	/// <param name="f">far</param>
	void SetNearFar(float n, float f);
	/// <summary>
	/// 視野角の設定
	/// </summary>
	/// <param name="angle">角度</param>
	void SetFov(float angle);
	/// <summary>
	/// 正面方向の設定
	/// </summary>
	void SetFront(const Vec3& front, float vertexAngle);
	/// <summary>
	/// 初期位置の設定
	/// </summary>
	/// <param name="targetPos">カメラの注視点</param>
	void SetTargetPos(const Vec3& targetPos);

	/// <summary>
	/// カメラの有効化
	/// </summary>
	void AppInfo();

	/// <summary>
	/// カメラの場所を取得
	/// </summary>
	/// <returns>pos</returns>
	const Vec3& GetPos() const;
	/// <summary>
	/// TPSかFPSか
	/// </summary>
	/// <returns>true:TPS /false: FPS</returns>
	bool IsTps() const { return m_info.isTps; }

protected:
	/// <summary>
	/// カメラの距離を取得
	/// </summary>
	/// <returns></returns>
	float GetDistance() const;

protected:
	// 情報
	CameraInfo m_info;
};

