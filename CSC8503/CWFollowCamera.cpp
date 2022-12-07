#include "CWFollowCamera.h"

NCL::CSC8503::CWFollowCamera::CWFollowCamera(Transform& target) : followTarget(target)
{
}

void NCL::CSC8503::CWFollowCamera::UpdateCamera(float dt)
{
	//position = followTarget.GetPosition() + followOffset;
	elaspedTime += dt;

	float camX = sin(elaspedTime) * 30.0f;
	float camZ = cos(elaspedTime) * 30.0f;
	position = Vector3(camX, 0.0f, camZ) + followTarget.GetPosition();

	Matrix4 viewMatrix = Matrix4::BuildViewMatrix(position, followTarget.GetPosition(), Vector3(0, 1, 0)).Inverse();
	Quaternion q(viewMatrix);
	Vector3 angles = q.ToEuler(); //nearly there now!

	pitch = angles.x;
	yaw = angles.y;
}