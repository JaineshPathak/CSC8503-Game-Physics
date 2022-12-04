#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "Window.h"
#include "Maths.h"
#include "Debug.h"

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;

		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) 
{
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVal(-1, -1, -1);

	for (int i = 0; i < 3; i++)
	{
		if (rayDir[i] > 0)
			tVal[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		else if (rayDir[i] < 0)
			tVal[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
	}

	float bestT = tVal.GetMaxElement();
	if (bestT < 0.0f)
		return false;

	Vector3 intersectionPoint = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;

	for (int i = 0; i < 3; i++)
	{
		if (intersectionPoint[i] + epsilon < boxMin[i] || intersectionPoint[i] - epsilon > boxMax[i])
			return false;
	}

	//Normal
	Vector3 intersectionPointDir = intersectionPoint - boxPos;
	Vector3 intersectionSurfaceNormal;

	/*float maxComp = abs(intersectionPointDir.x);
	Vector3 intersectionSurfaceNormal = Vector3(Sign(intersectionPointDir.x), 0.0f, 0.0f);
	if (abs(intersectionPointDir.y) > maxComp)
	{
		maxComp = abs(intersectionPointDir.y);
		intersectionSurfaceNormal = Vector3(0.0f, Sign(intersectionPointDir.y), 0.0f);
	}
	if (abs(intersectionPointDir.z) > maxComp)
	{
		maxComp = abs(intersectionPointDir.z);
		intersectionSurfaceNormal = Vector3(0.0f, 0.0f, Sign(intersectionPointDir.z));
	}*/
	float min = FLT_MAX;
	float distance = abs(boxSize.x - abs(intersectionPointDir.x));
	if (distance < min)
	{
		min = distance;
		intersectionSurfaceNormal = Vector3(1, 0, 0);
		intersectionSurfaceNormal *= Sign(intersectionPointDir.x);
	}
	distance = std::abs(boxSize.y - std::abs(intersectionPointDir.y));
	if (distance < min) {
		min = distance;
		intersectionSurfaceNormal = Vector3(0, 1, 0);
		intersectionSurfaceNormal *= Sign(intersectionPointDir.y);
	}
	distance = std::abs(boxSize.z - std::abs(intersectionPointDir.z));
	if (distance < min) {
		min = distance;
		intersectionSurfaceNormal = Vector3(0, 0, 1);
		intersectionSurfaceNormal *= Sign(intersectionPointDir.z);
	}

	collision.collidedAt = intersectionPoint;
	collision.collidedNormal = intersectionSurfaceNormal;
	collision.rayDistance = bestT;

	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) 
{
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();

	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) 
{
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided)
	{
		collision.collidedAt = transform * collision.collidedAt + position;
		collision.collidedNormal = transform * collision.collidedNormal;
	}

	return collided;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) 
{
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	Vector3 dir = (spherePos - r.GetPosition());

	float sphereProj = Vector3::Dot(dir, r.GetDirection());
	if (sphereProj < 0.0f)
		return false;

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);
	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius)
		return false;

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - (offset);
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	collision.collidedNormal = (collision.collidedAt - spherePos).Normalised();

	return true;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) 
{
	return false;
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	//Two AABBs
	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	//Two Spheres
	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	//Two OBBs
	if (pairType == VolumeType::OBB) {
		return OBBIntersection2((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	//Two Capsules

	//AABB vs Sphere pairs
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//OBB vs sphere pairs
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//Capsule vs other interactions
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		return AABBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	if (volB->type == VolumeType::Capsule && volA->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x &&
		abs(delta.y) < totalSize.y &&
		abs(delta.z) < totalSize.z) {
		return true;
	}
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) 
{
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);
	if (overlap)
	{
		static const Vector3 faces[6] =
		{
			Vector3(-1, 0, 0), Vector3(1, 0, 0),
			Vector3(0, -1, 0), Vector3(0, 1, 0),
			Vector3(0, 0, -1), Vector3(0, 0, 1)
		};

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] =
		{
			(maxB.x - minA.x),		//Distance of [box B] to left of [box a]
			(maxA.x - minB.x),		//Distance of [box B] to right of [box a]

			(maxB.y - minA.y),		//Distance of [box B] to bottom of [box a]
			(maxA.y - minB.y),		//Distance of [box B] to top of [box a]

			(maxB.z - minA.z),		//Distance of [box B] to front of [box a]
			(maxA.z - minB.z),		//Distance of [box B] to back of [box a]
		};

		float penetration = FLT_MAX;
		Vector3 bestAxis;

		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration)
			{
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}

		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}

	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	float deltaLength = delta.Length();

	if (deltaLength < radii)
	{
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}

	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
												const SphereVolume& volumeB, const Transform& worldTransformB, 
												CollisionInfo& collisionInfo)
{
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();
	//Debug::DrawLine(localPoint, localPoint + Vector3(0, 1, 0), Debug::BLACK, 10.0f);

	if (distance < volumeB.GetRadius())
	{
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		return true;
	}

	return false;
}

bool  CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
												const SphereVolume& volumeB, const Transform& worldTransformB, 
												CollisionInfo& collisionInfo)
{
	Matrix4 boxMat = worldTransformA.GetMatrix();
	//Debug::DrawAxisLines(boxMat, 3.0f, 1000.0);
	boxMat.SetPositionVector(Vector3(0, 0, 0));

	Vector3 boxRight = boxMat * Vector3(1, 0, 0);
	Vector3 boxUp = boxMat * Vector3(0, 1, 0);
	Vector3 boxForward = boxMat * Vector3(0, 0, -1);

	Vector3 u[3] = { boxRight.Normalised(), boxUp.Normalised(), boxForward.Normalised() };

	Vector3 boxCenter = worldTransformA.GetPosition();
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 sphereCenter = worldTransformB.GetPosition();
	
	//Closest Point
	Vector3 d = sphereCenter - boxCenter;
	Vector3 closestPoint = boxCenter;

	for (int i = 0; i < 3; i++)
	{
		float dist = Vector3::Dot(d, u[i]);
		if (dist > boxSize[i]) dist = boxSize[i];
		if (dist < -boxSize[i]) dist = -boxSize[i];
		
		closestPoint = closestPoint + (u[i] * dist);
	}

	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	Vector3 localPoint = delta - closestPoint;

	//Debug::DrawLine(closestPoint, closestPoint + Vector3(0, 1, 0), Debug::MAGENTA, 1000.0f);

	Vector3 v = closestPoint - sphereCenter;
	float radiusSq = volumeB.GetRadius() * volumeB.GetRadius();
	if (v.LengthSquared() <= radiusSq)
	{
		Vector3 collisionNormal = (sphereCenter - closestPoint).Normalised();
		float penetration = (volumeB.GetRadius() - v.Length());

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		return true;
	}

	return false;
}

bool CollisionDetection::AABBCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	return false;
}

bool CollisionDetection::SphereCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	return false;
}

//https://github.com/gszauer/GamePhysicsCookbook/blob/master/Code/Geometry3D.cpp
bool CollisionDetection::OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
										const OBBVolume& volumeB, const Transform& worldTransformB, 
										CollisionInfo& collisionInfo) 
{
	float ra, rb;
	Matrix3 R, AbsR;

	//------------------------------------------------------------------------------
	//Get Local Axis of OBB A
	Matrix4 boxAMat = worldTransformA.GetMatrix();
	boxAMat.SetPositionVector(Vector3(0, 0, 0));

	Vector3 boxARight = boxAMat * Vector3(1, 0, 0);
	Vector3 boxAUp = boxAMat * Vector3(0, 1, 0);
	Vector3 boxAForward = boxAMat * Vector3(0, 0, -1);

	Vector3 uA[3] = { boxARight.Normalised(), boxAUp.Normalised(), boxAForward.Normalised() };

	Vector3 boxAExtents = volumeA.GetHalfDimensions();
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	//Get Local Axis of OBB B
	Matrix4 boxBMat = worldTransformB.GetMatrix();
	boxBMat.SetPositionVector(Vector3(0, 0, 0));

	Vector3 boxBRight = boxBMat * Vector3(1, 0, 0);
	Vector3 boxBUp = boxBMat * Vector3(0, 1, 0);
	Vector3 boxBForward = boxBMat * Vector3(0, 0, -1);

	Vector3 uB[3] = { boxBRight.Normalised(), boxBUp.Normalised(), boxBForward.Normalised() };

	Vector3 boxBExtents = volumeB.GetHalfDimensions();
	//------------------------------------------------------------------------------

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R.array[i][j] = Vector3::Dot(uA[i], uB[j]);

	Vector3 t = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	t = Vector3(Vector3::Dot(t, uA[0]), Vector3::Dot(t, uA[1]), Vector3::Dot(t, uA[2]));

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR.array[i][j] = abs(R.array[i][j]) + 0.01f;

	//Test Axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = boxAExtents[i];
		rb = boxBExtents[0] * AbsR.array[i][0] +
			 boxBExtents[1] * AbsR.array[i][1] +
			 boxBExtents[2] * AbsR.array[i][2];

		if (abs(t[i]) > ra + rb)
			return false;
	}

	//Test Axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra = boxAExtents[0] * AbsR.array[0][i] + 
			 boxAExtents[1] * AbsR.array[1][i] + 
			 boxAExtents[2] * AbsR.array[2][i];
		rb = boxBExtents[i];

		if (abs(t[0] * R.array[0][i] + t[1] * R.array[1][i] + t[2] * R.array[2][i]) > ra + rb)
			return false;
	}



	//Test Axis L = A0 x B0
	ra = boxAExtents[1] * AbsR.array[2][0] + boxAExtents[2] * AbsR.array[1][0];
	rb = boxBExtents[1] * AbsR.array[0][2] + boxBExtents[2] * AbsR.array[0][1];
	if (abs(t[2] * R.array[1][0] - t[1] * R.array[2][0]) > ra + rb)
		return false;

	//Test Axis L = A0 x B1
	ra = boxAExtents[1] * AbsR.array[2][1] + boxAExtents[2] * AbsR.array[1][1];
	rb = boxBExtents[0] * AbsR.array[0][2] + boxBExtents[2] * AbsR.array[0][0];
	if (abs(t[2] * R.array[1][1] - t[1] * R.array[2][1]) > ra + rb)
		return false;

	//Test Axis L = A0 x B2
	ra = boxAExtents[1] * AbsR.array[2][2] + boxAExtents[2] * AbsR.array[1][2];
	rb = boxBExtents[0] * AbsR.array[0][1] + boxBExtents[1] * AbsR.array[0][0];
	if (abs(t[2] * R.array[1][2] - t[1] * R.array[2][2]) > ra + rb)
		return false;



	//Test Axis L = A1 x B0
	ra = boxAExtents[0] * AbsR.array[2][0] + boxAExtents[2] * AbsR.array[0][0];
	rb = boxBExtents[1] * AbsR.array[1][2] + boxBExtents[2] * AbsR.array[1][1];
	if (abs(t[0] * R.array[2][0] - t[2] * R.array[0][0]) > ra + rb)
		return false;

	//Test Axis L = A1 x B1
	ra = boxAExtents[0] * AbsR.array[2][1] + boxAExtents[2] * AbsR.array[0][1];
	rb = boxBExtents[0] * AbsR.array[1][2] + boxBExtents[2] * AbsR.array[1][0];
	if (abs(t[0] * R.array[2][1] - t[2] * R.array[0][1]) > ra + rb)
		return false;

	//Test Axis L = A1 x B2
	ra = boxAExtents[0] * AbsR.array[2][2] + boxAExtents[2] * AbsR.array[0][2];
	rb = boxBExtents[0] * AbsR.array[1][1] + boxBExtents[1] * AbsR.array[1][0];
	if (abs(t[0] * R.array[2][2] - t[2] * R.array[0][2]) > ra + rb)
		return false;




	//Test Axis L = A2 x B0
	ra = boxAExtents[0] * AbsR.array[1][0] + boxAExtents[1] * AbsR.array[0][0];
	rb = boxBExtents[1] * AbsR.array[2][2] + boxBExtents[2] * AbsR.array[2][1];
	if (abs(t[1] * R.array[0][0] - t[0] * R.array[1][0]) > ra + rb)
		return false;

	//Test Axis L = A2 x B1
	ra = boxAExtents[0] * AbsR.array[1][1] + boxAExtents[1] * AbsR.array[0][1];
	rb = boxBExtents[0] * AbsR.array[2][2] + boxBExtents[2] * AbsR.array[2][0];
	if (abs(t[1] * R.array[0][1] - t[0] * R.array[1][1]) > ra + rb)
		return false;

	//Test Axis L = A2 x B2
	ra = boxAExtents[0] * AbsR.array[1][2] + boxAExtents[1] * AbsR.array[0][2];
	rb = boxBExtents[0] * AbsR.array[2][1] + boxBExtents[1] * AbsR.array[2][0];
	if (abs(t[1] * R.array[0][2] - t[0] * R.array[1][2]) > ra + rb)
		return false;

	//Debug::DrawLine(t, t + Vector3(0, 1, 0), Debug::MAGENTA, 1000.0f);
	//RayCollision rayCollision;
	//Ray ray = Ray(worldTransformA.GetPosition(), worldTransformB.GetPosition() - worldTransformA.GetPosition());
	//if (RayOBBIntersection(ray, worldTransformB, volumeB, rayCollision))
	//{
	//	//Debug::DrawLine(rayCollision.collidedAt, rayCollision.collidedAt + rayCollision.collidedNormal, Debug::MAGENTA, 1000.0f);
	//	collisionInfo.AddContactPoint(Vector3(), rayCollision.collidedAt, -rayCollision.collidedNormal, rayCollision.rayDistance);
	//}

	ContactPoint points = FindOBBCollisionData(volumeA, worldTransformA, volumeB, worldTransformB);
	collisionInfo.AddContactPoint(points.localA, points.localB, points.normal, points.penetration);

	//std::cout << "There is an OBB-OBB collision\n";
	return true;
}

bool NCL::CollisionDetection::OBBIntersection2(const OBBVolume& volumeA, const Transform& worldTransformA, 
											   const OBBVolume& volumeB, const Transform& worldTransformB, 
											   CollisionInfo& collisionInfo)
{
	Matrix3 oA = Matrix3(worldTransformA.GetOrientation());
	Matrix3 oB = Matrix3(worldTransformB.GetOrientation());
	const float* o1 = oA.AsArray();
	const float* o2 = oB.AsArray();

	Vector3 test[15] = 
	{
		Vector3(o1[0], o1[1], o1[2]),
		Vector3(o1[3], o1[4], o1[5]),
		Vector3(o1[6], o1[7], o1[8]),
		Vector3(o2[0], o2[1], o2[2]),
		Vector3(o2[3], o2[4], o2[5]),
		Vector3(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis
		test[6 + i * 3 + 0] = Vector3::Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Vector3::Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Vector3::Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i) 
	{
		if (!OverlapOnAxis(volumeA, worldTransformA, volumeB, worldTransformB, test[i])) {
			return false; // Seperating axis found
		}
	}

	ContactPoint points = FindOBBCollisionData(volumeA, worldTransformA, volumeB, worldTransformB);
	collisionInfo.AddContactPoint(points.localA, points.localB, points.normal, points.penetration);
	//std::cout << "There is a collision!\n";
	return true; // Seperating axis not found
}

CollisionDetection::Interval CollisionDetection::GetInterval(const OBBVolume& obbVolume, const Transform& obbTransform, const Vector3& axis)
{
	Vector3 vertex[8];

	Vector3 C = obbTransform.GetPosition();	// OBB Center
	Vector3 E = obbVolume.GetHalfDimensions();		// OBB Extents

	Matrix3 obbOrientationMat = Matrix3(obbTransform.GetOrientation());
	const float* o = obbOrientationMat.AsArray();
	Vector3 A[] = {			// OBB Axis
		Vector3(o[0], o[1], o[2]),
		Vector3(o[3], o[4], o[5]),
		Vector3(o[6], o[7], o[8]),
	};

	vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	Interval result;
	result.min = result.max = Vector3::Dot(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = Vector3::Dot(axis, vertex[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}

float NCL::CollisionDetection::PenetrationDepth(const OBBVolume& volumeA, const Transform& worldTransformA, 
												const OBBVolume& volumeB, const Transform& worldTransformB, 
												const Vector3& axis, bool* outShouldFlip)
{
	Interval i1 = GetInterval(volumeA, worldTransformA, axis.Normalised());
	Interval i2 = GetInterval(volumeB, worldTransformB, axis.Normalised());

	if (!((i2.min <= i1.max) && (i1.min <= i2.max))) 
	{
		return 0.0f; // No penerattion
	}

	float len1 = i1.max - i1.min;
	float len2 = i2.max - i2.min;
	float min = fminf(i1.min, i2.min);
	float max = fmaxf(i1.max, i2.max);
	float length = max - min;

	if (outShouldFlip != 0) {
		*outShouldFlip = (i2.min < i1.min);
	}

	return (len1 + len2) - length;
}

CollisionDetection::ContactPoint CollisionDetection::FindOBBCollisionData(const OBBVolume& volumeA, const Transform& worldTransformA, 
																			const OBBVolume& volumeB, const Transform& worldTransformB)
{
	ContactPoint result;
	result.localA = Vector3();
	result.localB = Vector3();
	result.normal = Vector3(0, 0, 1);
	result.penetration = FLT_MAX;

	Matrix3 AOrientationMat = Matrix3(worldTransformA.GetOrientation());
	Matrix3 BOrientationMat = Matrix3(worldTransformB.GetOrientation());

	const float* o1 = AOrientationMat.AsArray();
	const float* o2 = BOrientationMat.AsArray();

	Vector3 test[15] = 
	{
		Vector3(o1[0], o1[1], o1[2]),
		Vector3(o1[3], o1[4], o1[5]),
		Vector3(o1[6], o1[7], o1[8]),
		Vector3(o2[0], o2[1], o2[2]),
		Vector3(o2[3], o2[4], o2[5]),
		Vector3(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i) 
	{	// Fill out rest of axis
		test[6 + i * 3 + 0] = Vector3::Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Vector3::Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Vector3::Cross(test[i], test[2]);
	}

	Vector3* hitNormal = 0;
	bool shouldFlip;

	for (int i = 0; i < 15; ++i)
	{
		if (test[i].x < 0.000001f) test[i].x = 0.0f;
		if (test[i].y < 0.000001f) test[i].y = 0.0f;
		if (test[i].z < 0.000001f) test[i].z = 0.0f;
		if (test[i].LengthSquared() < 0.001f) {
			continue;
		}

		float depth = PenetrationDepth(volumeA, worldTransformA, volumeB, worldTransformB, test[i], &shouldFlip);
		if (depth <= 0.0f) 
		{
			return result;
		}
		else if (depth < result.penetration) 
		{
			if (shouldFlip) 
			{
				test[i] = test[i] * -1.0f;
			}
			result.penetration = depth;
			hitNormal = &test[i];
		}
	}

	if (hitNormal == 0)
		return result;

	Vector3 axis = (*hitNormal).Normalised();

	std::vector<Vector3> allContacts;
	std::vector<Vector3> c1 = ClipEdgesToOBB(GetEdges(volumeB, worldTransformB), volumeA, worldTransformA);
	std::vector<Vector3> c2 = ClipEdgesToOBB(GetEdges(volumeA, worldTransformA), volumeB, worldTransformB);
	allContacts.reserve(c1.size() + c2.size());
	allContacts.insert(allContacts.end(), c1.begin(), c1.end());
	allContacts.insert(allContacts.end(), c2.begin(), c2.end());

	Interval i = GetInterval(volumeA, worldTransformA, axis);
	float distance = (i.max - i.min) * 0.5f - result.penetration * 0.5f;
	Vector3 pointOnPlane = worldTransformA.GetPosition() + axis * distance;

	for (int i = allContacts.size() - 1; i >= 0; --i) 
	{
		Vector3 contact = allContacts[i];
		allContacts[i] = contact + (axis * Vector3::Dot(axis, pointOnPlane - contact));

		for (int j = allContacts.size() - 1; j > i; --j) 
		{
			if ((allContacts[j] - allContacts[i]).LengthSquared() < 0.0001f)
			{
				allContacts.erase(allContacts.begin() + j);
				break;
			}
		}
	}

	if (allContacts.size() == 2)
	{
		result.localA = allContacts[0];
		result.localB = allContacts[1];
	}
	result.normal = axis;

	return result;
}

std::vector<Vector3> NCL::CollisionDetection::GetVertices(const OBBVolume& obbVolume, const Transform& obbTransform)
{
	std::vector<Vector3> v;
	v.resize(8);

	Vector3 C = obbTransform.GetPosition();	// OBB Center
	Vector3 E = obbVolume.GetHalfDimensions();		// OBB Extents


	Matrix3 obbOrientationMat = Matrix3(obbTransform.GetOrientation());
	const float* o = obbOrientationMat.AsArray();
	Vector3 A[] = 
	{			// OBB Axis
		Vector3(o[0], o[1], o[2]),
		Vector3(o[3], o[4], o[5]),
		Vector3(o[6], o[7], o[8]),
	};

	v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	return v;
}

std::vector<CollisionDetection::Line> CollisionDetection::GetEdges(const OBBVolume& obbVolume, const Transform& obbTransform)
{
	std::vector<Line> result;
	result.reserve(12);
	std::vector<Vector3> v = GetVertices(obbVolume, obbTransform);

	int index[][2] = 
		{ // Indices of edges
		{ 6, 1 },{ 6, 3 },{ 6, 4 },{ 2, 7 },{ 2, 5 },{ 2, 0 },
		{ 0, 1 },{ 0, 3 },{ 7, 1 },{ 7, 4 },{ 4, 5 },{ 5, 3 }
	};

	for (int j = 0; j < 12; ++j) 
	{
		result.push_back(Line(
			v[index[j][0]], v[index[j][1]]
		));
	}

	return result;
}

std::vector<Vector3> NCL::CollisionDetection::ClipEdgesToOBB(const std::vector<Line>& edges, const OBBVolume& obbVolume, const Transform& obbTransform)
{
	std::vector<Vector3> result;
	result.reserve(edges.size() * 3);
	Vector3 intersection;

	const std::vector<Plane>& planes = GetPlanes(obbVolume, obbTransform);

	for (int i = 0; i < planes.size(); ++i) 
	{
		for (int j = 0; j < edges.size(); ++j) 
		{
			if (ClipToPlane(planes[i], edges[j], &intersection)) 
			{
				if (PointInOBB(intersection, obbVolume, obbTransform))
				{
					result.push_back(intersection);
				}
			}
		}
	}

	return result;
}

bool NCL::CollisionDetection::PointInOBB(const Vector3& point, const OBBVolume& obbVolume, const Transform& obbTransform)
{
	Vector3 dir = point - obbTransform.GetPosition();

	Vector3 obbSize = obbVolume.GetHalfDimensions();
	for (int i = 0; i < 3; ++i) 
	{
		Matrix3 obbOrientationMat = Matrix3(obbTransform.GetOrientation());
		const float* orientation = obbOrientationMat.AsArray();
		//float orientationMain = orientation[i * 3];
		Vector3 axis(orientation[0], orientation[3], orientation[6]);

		float distance = Vector3::Dot(dir, axis);

		if (distance > obbSize[i]) {
			return false;
		}
		if (distance < -obbSize[i]) {
			return false;
		}
	}

	return true;
}

std::vector<Plane> NCL::CollisionDetection::GetPlanes(const OBBVolume& obbVolume, const Transform& obbTransform)
{
	Vector3 c = obbTransform.GetPosition();	// OBB Center
	Vector3 e = obbVolume.GetHalfDimensions();		// OBB Extents

	Matrix3 obbOrientationMat = Matrix3(obbTransform.GetOrientation());
	const float* o = obbOrientationMat.AsArray();
	Vector3 a[] = {			// OBB Axis
		Vector3(o[0], o[1], o[2]),
		Vector3(o[3], o[4], o[5]),
		Vector3(o[6], o[7], o[8]),
	};

	std::vector<Plane> result;
	result.resize(6);

	result[0] = Plane(a[0], Vector3::Dot(a[0], (c + a[0] * e.x)));
	result[1] = Plane(a[0] * -1.0f, -Vector3::Dot(a[0], (c - a[0] * e.x)));
	result[2] = Plane(a[1], Vector3::Dot(a[1], (c + a[1] * e.y)));
	result[3] = Plane(a[1] * -1.0f, -Vector3::Dot(a[1], (c - a[1] * e.y)));
	result[4] = Plane(a[2], Vector3::Dot(a[2], (c + a[2] * e.z)));
	result[5] = Plane(a[2] * -1.0f, -Vector3::Dot(a[2], (c - a[2] * e.z)));

	return result;
}

bool NCL::CollisionDetection::ClipToPlane(const Plane& plane, const Line& line, Vector3* outPoint)
{
	Vector3 ab = line.end - line.start;

	float nA = Vector3::Dot(plane.GetNormal(), line.start);
	float nAB = Vector3::Dot(plane.GetNormal(), ab);

	if (AlmostEqualRelativeAndAbs(nAB, 0, 0.005f)) {
		return false;
	}

	float t = (plane.GetDistance() - nA) / nAB;
	if (t >= 0.0f && t <= 1.0f) {
		if (outPoint != 0) {
			*outPoint = line.start + ab * t;
		}
		return true;
	}

	return false;
}

bool NCL::CollisionDetection::OverlapOnAxis(const OBBVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, const Vector3& axis)
{
	Interval a = GetInterval(volumeA, worldTransformA, axis);
	Interval b = GetInterval(volumeB, worldTransformB, axis);

	return ((b.min <= a.max) && (a.min <= b.max));
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Matrix4 GenerateInverseProjection(float aspect, float nearPlane, float farPlane, float fov) {
	float negDepth = nearPlane - farPlane;

	float invNegDepth = negDepth / (2 * (farPlane * nearPlane));

	Matrix4 m;

	float h = 1.0f / tan(fov*PI_OVER_360);

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = invNegDepth;//// +PI_OVER_360;
	m.array[3][2] = -1.0f;
	m.array[3][3] = (0.5f / nearPlane) + (0.5f / farPlane);

	return m;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 proj  = cam.BuildProjectionMatrix(aspect);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition(), c);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam, const Vector3& offset)
{
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition() + offset, c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = 1.0f / d;

	m.array[3][2] = 1.0f / e;
	m.array[3][3] = -c / (d * e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(pitch, Vector3(1, 0, 0));

	return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

