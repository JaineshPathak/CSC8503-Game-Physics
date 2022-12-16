#include "CWPawn.h";
#include "PhysicsObject.h"

void NCL::CSC8503::CWPawn::MoveTowards(Vector3 src, const Vector3& pos, float dt)
{
	auto color = Debug::BLUE;
	if ((previousPosition - transform.GetPosition()).Length() < (distanceThreshold * 0.01)) {
		src = transform.GetPosition();
		color = Debug::MAGENTA;
	}

	//Debug::DrawLine(pos, src, color);
	auto v = (pos - src).Normalised();
	v.y = 0.0f;
	physicsObject->AddForce(v * moveSpeed);
	//v = (v).Normalised() * moveSpeed;
	//physicsObject->SetLinearVelocity(v);

	previousPosition = transform.GetPosition();

	//transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWPawn::MoveTowards(const Vector3& pos, float dt, bool useForce)
{
	if (useForce)
	{
		auto v = (pos - transform.GetPosition()).Normalised();
		v.y = 0.0f;
		physicsObject->AddForce(v * moveSpeed);
	}
	else
		transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWPawn::RotateTowards(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}

void NCL::CSC8503::CWPawn::RotateAway(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.y -= 180.0f;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}

void NCL::CSC8503::CWPawn::FindPath(const Vector3& destination, std::vector<Vector3>& _pathList)
{
	if (goatGame.GetNavGrid()->FindPath(transform.GetPosition(), destination, path))
	{
		if (_pathList.size() > 0) _pathList.clear();

		Vector3 pos;
		while (path.PopWaypoint(pos))
			_pathList.push_back(pos);
	}
}