#include "CWPropDestroy.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWPropDestroy::CWPropDestroy(CWGoatGame& gGame, const Vector3& pos, const Vector3& baseYPos, const Vector3& scale, const Vector3& boxSize, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader) : goatGame(gGame)
{
	AABBVolume* volume = new AABBVolume(boxSize);
	SetBoundingVolume((CollisionVolume*)volume);

	transform
		.SetPosition(pos)
		.SetScale(scale * 2);

	renderObject = new RenderObject(&transform, mesh, texture, shader);
	renderObject->SetColour(Debug::WHITE);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(0.5f);	//40.0f
	physicsObject->InitCubeInertia();
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	basePos = baseYPos;
	//Debug::DrawBox(transform.GetPosition() + basePos, Vector3(1, 1, 1), Debug::YELLOW, 1000.0f);

	goatGame.OnPropSpawn();
}

void NCL::CSC8503::CWPropDestroy::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject == goatGame.GetPlayer())
	{
		physicsObject->SetInverseMass(40.0f);
		Vector3 dir = goatGame.GetPlayer()->GetTransform().GetPosition() - (transform.GetPosition() + basePos);
		physicsObject->AddForceAtPosition(-dir * 20.0f, goatGame.GetPlayer()->GetTransform().GetPosition());

		if (isDestroyed)
			return;

		isDestroyed = true;
		physicsObject->SetRestitution(0.5);
		renderObject->SetColour(Vector4(0.2f, 0.2f, 0.2f, 1.0f));

		goatGame.OnPropDestroy(scoreAdd);
	}
}