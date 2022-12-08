#include "CWFollowCamera.h"
#include "Window.h"
#include "Maths.h"
//#include <iostream>

NCL::CSC8503::CWFollowCamera::CWFollowCamera(GameWorld& gWorld, GameObject& target) : world(gWorld), followTarget(target)
{
	requiredRayDistance = 36.0f;
	startOffset = followOffset;
}

void NCL::CSC8503::CWFollowCamera::UpdateCamera(float dt)
{
	//position = followTarget.GetPosition() + followOffset;

	/*elaspedTime += dt;

	float camX = sin(pitch) * 30.0f;
	float camZ = cos(elaspedTime) * 30.0f;
	position = Vector3(camX, 0.0f, camZ) + followTarget.GetPosition();

	Matrix4 viewMatrix = Matrix4::BuildViewMatrix(position, followTarget.GetPosition(), Vector3(0, 1, 0)).Inverse();
	Quaternion q(viewMatrix);
	Vector3 angles = q.ToEuler();

	pitch = angles.x;
	yaw = angles.y;*/

	v -= (Window::GetMouse()->GetRelativePosition().y);
	v = Clamp(v, -40.0f, 45.0f);

	h -= (Window::GetMouse()->GetRelativePosition().x);
	if (h < 0) h += 360.0f;
	if (h > 360.0f) h -= 360.0f;


	//Quaternion turnAngle = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), h);
	//Quaternion turnAngleY = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), v);

	Matrix4 yawMat = Matrix4::Rotation(h, Vector3(0, 1, 0));
	Matrix4 pitchMat = Matrix4::Rotation(v, yawMat * Vector3(-1, 0, 0));
	Matrix4 finalRotMat = pitchMat * yawMat;

	Vector3 focusPoint = followTarget.GetTransform().GetPosition();
	Vector3 lookDirection = finalRotMat * Vector3(0, 0, -1);

	position = focusPoint - lookDirection * requiredRayDistance;

	//Prevent Wall Clipping
	Ray collisionRay = Ray(focusPoint, -lookDirection);
	RayCollision collisionRayData;
	if (world.Raycast(collisionRay, collisionRayData, true, &followTarget))
	{
		if(collisionRayData.rayDistance < requiredRayDistance)
			position = focusPoint - lookDirection * (collisionRayData.rayDistance - 1.0f);
	}

	/*Vector3 rayDir = finalRotMat * Vector3(0, 0, -1);
	Ray collisionRay = Ray(followTarget.GetTransform().GetPosition(), -rayDir);
	RayCollision collisionData;
	if (world.Raycast(collisionRay, collisionData, true, &followTarget))
	{
		Vector3 collideOffset = collisionData.collidedAt - followTarget.GetTransform().GetPosition();
		if (collideOffset.Length() <= requiredRayDistance)
		{
			Debug::DrawBox(collisionData.collidedAt, Vector3(1, 1, 1), Debug::WHITE);
			followOffset = finalRotMat * startOffset;
			position = followTarget.GetTransform().GetPosition() - rayDir * collisionData.rayDistance;
		}
		else
		{
			followOffset = finalRotMat * startOffset;
			position = followTarget.GetTransform().GetPosition() + followOffset;
		}
	}
	else
	{
		followOffset = finalRotMat * startOffset;
		position = followTarget.GetTransform().GetPosition() + followOffset;
	}*/

	//followOffset = finalRotMat * Vector3(0, 20.0f, 30.0f);
	//position = followTarget.GetTransform().GetPosition() + followOffset;

	//float distance = (position - followTarget.GetPosition()).Length();
	//std::cout << distance << std::endl;


	//Stare at GOAT!
	Matrix4 viewMatrix = Matrix4::BuildViewMatrix(position, followTarget.GetTransform().GetPosition(), Vector3(0, 1, 0)).Inverse();
	Quaternion q(viewMatrix);
	pitch = q.ToEuler().x + pitchOffset;
	yaw = q.ToEuler().y;
}