#include "CWDoorTrigger.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWDoorTrigger::CWDoorTrigger(CWGoatGame& gGame, const Vector3& wallSize) : goatGame(gGame)
{
	AABBVolume* volume = new AABBVolume(wallSize);
	SetBoundingVolume((CollisionVolume*)volume);

	physicsObject = new PhysicsObject(&transform, boundingVolume, true);
	physicsObject->SetInverseMass(0.0f);
	physicsObject->InitCubeInertia();
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);
}

void NCL::CSC8503::CWDoorTrigger::OnTriggerBegin(GameObject* otherObject)
{
	if (isTriggered) return;

	if (otherObject->GetTag() == "Player" && !isTriggered)
	{
		CWGoatPlayer* player = dynamic_cast<CWGoatPlayer*>(otherObject);
		isTriggered = (player != nullptr) && player->hasDoorKey;

		if (isTriggered && warehouseDoor != nullptr)
		{
			Vector3 doorPos = warehouseDoor->GetTransform().GetPosition();
			doorPos.y = 75.0f;
			warehouseDoor->GetTransform().SetPosition(Vector3(doorPos));
		}
	}
}
