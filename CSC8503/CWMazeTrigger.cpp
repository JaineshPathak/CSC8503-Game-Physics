#include "CWMazeTrigger.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWMazeTrigger::CWMazeTrigger(CWGoatGame& gGame, const Vector3& wallSize) : goatGame(gGame)
{
	AABBVolume* volume = new AABBVolume(wallSize);
	SetBoundingVolume((CollisionVolume*)volume);

	physicsObject = new PhysicsObject(&transform, boundingVolume, true);
	physicsObject->SetInverseMass(0.0f);
	physicsObject->InitCubeInertia();
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);
}

void NCL::CSC8503::CWMazeTrigger::OnTriggerBegin(GameObject* otherObject)
{
	if (otherObject == goatGame.GetPlayer() && !playerEntered)
	{
		goatGame.GetPlayer()->GetRenderObject()->SetColour(Debug::MAGENTA);
		playerEntered = true;
		//wasPlayerEntered = false;
	}
}

void NCL::CSC8503::CWMazeTrigger::OnTriggerEnd(GameObject* otherObject)
{
	if (otherObject == goatGame.GetPlayer() && playerEntered)
	{
		goatGame.GetPlayer()->GetRenderObject()->SetColour(Debug::WHITE);
		playerEntered = false;
		//wasPlayerEntered = true;
	}
}
