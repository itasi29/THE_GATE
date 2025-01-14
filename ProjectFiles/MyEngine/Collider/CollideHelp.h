﻿#pragma once
#include "Geometry/Vec3.h"
#include "Collider/ColliderBase.h"

// FIXME: ここのサマリーを書く

namespace MyEngine
{
	class ColliderSphere;
	class ColliderCapsule;
	class ColliderLine;
	class ColliderBox;

	CollideHitInfo IsCollideSphereAndSphere(const Vec3& posA, const Vec3& rigidB, ColliderSphere* colliderA, ColliderSphere* colliderB);
	CollideHitInfo IsCollideSphereAndCapsule(const Vec3& posSphere, const Vec3& posCapsule, ColliderSphere* colliderSphere, ColliderCapsule* colliderCapsule);
	CollideHitInfo IsCollideSphereAndBox(const Vec3& posSphere, const Vec3& posBox, ColliderSphere* colliderSphere, ColliderBox* colliderBox, const PreHitInfo& preInfo);
	CollideHitInfo IsCollideCapsuleAndBox(const Vec3& posCapsule, const Vec3& posBox, ColliderCapsule* colliderCapsule, ColliderBox* colliderBox, const PreHitInfo& preInfo);
	CollideHitInfo IsCollideCapsuleAndCapsule(const Vec3& posA, const Vec3& posB, ColliderCapsule* colliderA, ColliderCapsule* colliderB);

	Vec3 FixSphereToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);
	Vec3 FixCapsuleToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);
	Vec3 FixSphereToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);
	Vec3 FixCapsuleToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);
	Vec3 FixBoxToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);
	Vec3 FixBoxToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);
}