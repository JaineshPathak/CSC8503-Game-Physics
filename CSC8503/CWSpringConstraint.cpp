#include "CWSpringConstraint.h"
#include "GameObject.h"

using namespace NCL;
using namespace Maths;
using namespace CSC8503;

NCL::CSC8503::CWSpringConstraint::CWSpringConstraint(GameObject* a, Vector3 aP, float d, float sF, float sD)
{
	objectA = a;
	anchorPoint = aP;
	//objectB = b;

	distance = d;
	springForce = sF;
	springDamping = sD;
}

NCL::CSC8503::CWSpringConstraint::~CWSpringConstraint()
{
}

void NCL::CSC8503::CWSpringConstraint::UpdateConstraint(float dt)
{
	PhysicsObject* physO = objectA->GetPhysicsObject();

	/*float k = springForce;
	float b = springDamping;
	Vector3 x = objectA->GetTransform().GetPosition() - anchorPoint;

	Vector3 velAdd = physO->GetLinearVelocity() + physO->GetAngularVelocity();

	Vector3 v = Vector3::Cross(velAdd, anchorPoint - objectA->GetTransform().GetPosition());
	Vector3 desiredX = x.Normalised() * (x.Length() - distance);
	Vector3 force = desiredX * (-k) - (v * b);*/

	Vector3 difference = objectA->GetTransform().GetPosition() - anchorPoint;
	float currentDistance = difference.Length();
	float originalDistance = distance;
	float displacement = (currentDistance - originalDistance) / currentDistance;
	Vector3 force = difference * displacement * (-springForce);
	force -= difference.Normalised() * springDamping;

	//-k * x;
	//std::cout << force << std::endl;

	physO->ApplyLinearImpulse(force * dt);
	//physO->AddForce(force);
	//physO->AddTorque(Vector3::Cross(force, anchorPoint - objectA->GetTransform().GetPosition()));
}