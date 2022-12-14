#include "CWPropDestroy.h"
#include "CWGoatGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

NCL::CSC8503::CWPropDestroy::CWPropDestroy(CWGoatGame& gGame, const Vector3& pos, const Vector3& baseYPos, const Vector3& scale, const Vector3& boxSize, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader) : goatGame(gGame)
{
	tag = "Prop";

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

	defaultPos = transform.GetPosition();
	defaultInvMass = physicsObject->GetInverseMass();
	//Debug::DrawBox(transform.GetPosition() + basePos, Vector3(1, 1, 1), Debug::YELLOW, 1000.0f);

	goatGame.OnPropSpawn(this);
}

void NCL::CSC8503::CWPropDestroy::OnCollisionBegin(GameObject* otherObject)
{
	if (firstTime)
	{
		defaultRot = transform.GetOrientation().ToEuler();
		defaultColor = renderObject->GetColour();
		
		firstTime = false;
	}

	if (otherObject->GetTag() == "Player")
	{
		if (goatGame.GetGameState() == GameState::GameEnded)
			return;

		physicsObject->SetInverseMass(40.0f);
		Vector3 dir = goatGame.GetPlayer()->GetTransform().GetPosition() - (transform.GetPosition() + basePos);
		physicsObject->AddForceAtPosition(-dir * 20.0f, goatGame.GetPlayer()->GetTransform().GetPosition());

		if (isDestroyed)
			return;

		isDestroyed = true;
		physicsObject->SetRestitution(0.5);
		renderObject->SetColour(Vector4(0.2f, 0.2f, 0.2f, 1.0f));

		goatGame.OnPropDestroy(scoreAdd);

		CWGoatPlayer* player = dynamic_cast<CWGoatPlayer*>(otherObject);
		if (player != nullptr)
			player->AddScore(scoreAdd);
	}
}

void NCL::CSC8503::CWPropDestroy::ResetProp()
{
	physicsObject->SetLinearVelocity(Vector3(0, 0, 0));
	physicsObject->SetAngularVelocity(Vector3(0, 0, 0));
	physicsObject->SetInverseMass(defaultInvMass);

	transform.SetPosition(defaultPos);
	transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(defaultRot.x, defaultRot.y, defaultRot.z));

	renderObject->SetColour(defaultColor);

	isDestroyed = false;
}
