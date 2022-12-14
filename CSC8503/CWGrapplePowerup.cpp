#include "CWGrapplePowerup.h"
#include "CWGoatPlayer.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "CWGoatGame.h"

NCL::CSC8503::CWGrapplePowerup::CWGrapplePowerup(CWGoatGame& gGame, GameWorld& gWorld, const Vector3& pos,
												MeshGeometry* mesh,
												TextureBase* tex,
												ShaderBase* shader,
												const Vector4& color)
{
	tag = "Powerup";

	AABBVolume* volume = new AABBVolume(powerupScale);
	SetBoundingVolume((CollisionVolume*)volume);

	transform
		.SetPosition(pos)
		.SetScale(powerupScale * 2);

	defaultPos = transform.GetPosition();

	renderObject = new RenderObject(&transform, mesh, tex, shader);
	renderObject->SetColour(color);

	physicsObject = new PhysicsObject(&transform, boundingVolume, true);
	physicsObject->SetInverseMass(0.0f);
	physicsObject->InitCubeInertia();
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	gGame.AddPowerupToList(this);
}

void NCL::CSC8503::CWGrapplePowerup::Update(float dt)
{
	elaspedTime += dt;

	if (currentState == PowerUpState::Cooldown)
	{
		respawnTimeCurrent -= dt;
		if (respawnTimeCurrent <= 0.0f)
		{
			respawnTimeCurrent = 0.0f;
			currentState = PowerUpState::Active;

			transform.SetPosition(defaultPos);
			transform.SetScale(powerupScale * 2.0f);
		}
	}
	else if (currentState == PowerUpState::Active)
	{
		Vector3 currentPos = transform.GetPosition();
		currentPos.y += sin(elaspedTime * 5.0f) / 12.0f;
		transform.SetPosition(currentPos);

		yRot += dt;
		if (yRot < 0) yRot += 360.0f;
		if (yRot > 360.0f) yRot -= 360.0f;
		transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, yRot * rotationSpeed, 0));
	}
}

void NCL::CSC8503::CWGrapplePowerup::OnTriggerBegin(GameObject* otherObject)
{
	if (otherObject->GetTag() == "Player" && currentState == PowerUpState::Active)
	{
		CWGoatPlayer* player = dynamic_cast<CWGoatPlayer*>(otherObject);
		if (player != nullptr)
		{
			player->GiveRopePowerup(powerupTime);

			respawnTimeCurrent = respawnTime;
			transform.SetScale(Vector3(0, 0, 0));
			currentState = PowerUpState::Cooldown;
		}
	}
}
