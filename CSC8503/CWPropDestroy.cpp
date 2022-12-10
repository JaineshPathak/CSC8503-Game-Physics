#include "CWPropDestroy.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWPropDestroy::CWPropDestroy(CWGoatGame& gGame, const Vector3& pos, const Vector3& size, const Vector3& boxSize, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader) : goatGame(gGame)
{
	AABBVolume* volume = new AABBVolume(boxSize);
	SetBoundingVolume((CollisionVolume*)volume);

	transform
		.SetPosition(pos)
		.SetScale(size * 2);

	renderObject = new RenderObject(&transform, mesh, texture, shader);
	renderObject->SetColour(Debug::WHITE);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(0.0f);
	physicsObject->InitCubeInertia();

	goatGame.OnPropSpawn();
}

void NCL::CSC8503::CWPropDestroy::OnCollisionBegin(GameObject* otherObject)
{
	if (isDestroyed)
		return;

	if (otherObject == goatGame.GetPlayer())
	{
		isDestroyed = true;
		physicsObject->SetInverseMass(20.0f);
		renderObject->SetColour(Vector4(0.2f, 0.2f, 0.2f, 1.0f));

		goatGame.OnPropDestroy(scoreAdd);
	}
}