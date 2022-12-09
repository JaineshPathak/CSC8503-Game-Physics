#include "CWJumpPad.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWJumpPad::CWJumpPad(CWGoatGame& gGame, Vector3 position, Vector3 size, float jumpForce, Vector4 color, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader) : goatGame(gGame)
{
	this->jumpDir = transform.Up();
	this->jumpForce = jumpForce;

	OBBVolume* volume = new OBBVolume(size);
	SetBoundingVolume((CollisionVolume*)volume);

	transform
		.SetPosition(position)
		.SetScale(size * 2);

	renderObject = new RenderObject(&transform, mesh, texture, shader);
	renderObject->SetColour(color);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(0);
	physicsObject->InitCubeInertia();
}

void NCL::CSC8503::CWJumpPad::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject == goatGame.GetPlayer())
		otherObject->GetPhysicsObject()->ApplyLinearImpulse(jumpDir * jumpForce);
}
