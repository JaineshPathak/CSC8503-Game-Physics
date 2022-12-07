#include "CWFollowCamera.h"
#include "Window.h"

NCL::CSC8503::CWFollowCamera::CWFollowCamera(Transform& target) : followTarget(target)
{
	//followOffset = followTarget.GetPosition() - position;
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
	h -= (Window::GetMouse()->GetRelativePosition().x);

	//Quaternion turnAngle = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), h);
	//Quaternion turnAngleY = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), v);

	Matrix4 yawMat = Matrix4::Rotation(h, Vector3(0, 1, 0));
	Matrix4 pitchMat = Matrix4::Rotation(v, yawMat * Vector3(-1, 0, 0));

	Matrix4 finalRotMat = pitchMat * yawMat;

	followOffset = finalRotMat * Vector3(0, 20.0f, 30.0f);
	position = followTarget.GetPosition() + followOffset;




	//Stare at GOAT!
	Matrix4 viewMatrix = Matrix4::BuildViewMatrix(position, followTarget.GetPosition(), Vector3(0, 1, 0)).Inverse();
	Quaternion q(viewMatrix);
	pitch = q.ToEuler().x + pitchOffset;
	yaw = q.ToEuler().y;
}