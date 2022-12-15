#include "CWGoatPlayer.h"
#include "CWGoatGame.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Maths.h"
#include "CWSpringConstraint.h"

NCL::CSC8503::CWGoatPlayer::CWGoatPlayer(CWGoatGame& gGame, GameWorld& gWorld, GameTechRenderer& gRenderer) : game(gGame), world(gWorld), renderer(gRenderer)
{
	tag = "Player";

	currentHealth = maxHealth;

	goatMesh = renderer.LoadMesh("Goat.msh");
	whiteTex = renderer.LoadTexture("WhiteTex.png");
	basicShader = renderer.LoadShader("scene.vert", "scene.frag");

	SphereVolume* volume = new SphereVolume(5.0f);
	SetBoundingVolume((CollisionVolume*)volume);

	transform
		.SetPosition(startPos)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(startRot.x, startRot.y, startRot.z))
		.SetScale(startScale * 2);

	renderObject = new RenderObject(&transform, goatMesh, whiteTex, basicShader);
	renderObject->SetColour(Debug::WHITE);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(mass);
	physicsObject->InitCubeInertia();
	physicsObject->SetLinearDamping(0.3f);
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	springRope = new CWSpringConstraint(this, Vector3(0, 0, 0), 0.0f, ropeForce, ropeDamping);
	springRope->isEnabled = false;
	world.AddConstraint(springRope);

	world.AddGameObject(this);
}

NCL::CSC8503::CWGoatPlayer::~CWGoatPlayer()
{
	delete goatMesh;
	delete whiteTex;
	delete basicShader;

	delete springRope;
}

void NCL::CSC8503::CWGoatPlayer::Update(float dt)
{
	Ray groundRay = Ray(transform.GetPosition(), Vector3(0, -1.0f, 0));
	RayCollision groundCollisionData;
	if (world.Raycast(groundRay, groundCollisionData, true, this))
	{
		float distance = (groundCollisionData.collidedAt - transform.GetPosition()).Length();
		isOnGround = (distance <= 6.0f);
	}

	//renderObject->SetColour(isOnGround ? Debug::BLUE : Debug::RED);
	//physicsObject->SetLinearDamping(isOnGround ? 3.0f : 0.1f);

	if (game.GetCursorStatus() || game.GetGameState() == GameState::GameEnded)
		return;

	Vector3 linearMovement;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::W)) linearMovement.z = -1.0f;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::S)) linearMovement.z = 1.0f;

	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::A)) linearMovement.x = -1.0f;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::D)) linearMovement.x = 1.0f;

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isOnGround) physicsObject->AddForce(Vector3(0, 1, 0) * jumpForce * 50.0f);

	if (ropePowerupCurrent > 0.0)
	{
		ropePowerupCurrent -= dt;
		if (ropePowerupCurrent <= 0.0f) ropePowerupCurrent = 0.0f;

		Debug::Print("Grapple: " + std::to_string((int)ropePowerupCurrent), Vector2(2, 95), Debug::YELLOW);
	}
	ropePowerup = (ropePowerupCurrent > 0.0f);
	if(ropePowerup) Debug::Print("x", Vector2(49.5f, 49.5f), Debug::YELLOW);
	if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT) && ropePowerup)
	{
		isHooked = !isHooked;
		if (isHooked && !springRope->isEnabled)
		{
			Ray camRay = Ray(world.GetMainCamera()->GetPosition(), world.GetMainCamera()->GetForward());
			RayCollision camRayCollisionData;
			if (world.Raycast(camRay, camRayCollisionData, true, this))
			{
				float ropeDistance = (transform.GetPosition() - groundCollisionData.collidedAt).Length();
				ropeAnchorPoint = camRayCollisionData.collidedAt;

				springRope->NewData(ropeAnchorPoint, ropeDistance, ropeForce, ropeDamping);
				springRope->isEnabled = true;
			}
			else
				isHooked = false;
		}
		else if (!isHooked && springRope != nullptr)
			springRope->isEnabled = false;
	}

	if (isHooked)
	{
		if (!ropePowerup && (springRope != nullptr && springRope->isEnabled))
		{
			springRope->isEnabled = false;
			isHooked = false;
		}

		Debug::DrawLine(transform.GetPosition(), ropeAnchorPoint, Debug::BLACK);
	}

	//airControl = isOnGround ? 1.0f : 0.1f;

	linearMovement.y = 0.0f;
	if (linearMovement.Length() >= 0.1f)
	{
		float targetAngle = atan2(-linearMovement.x, -linearMovement.z);
		targetAngle = RadiansToDegrees(targetAngle) + world.GetMainCamera()->GetYaw();

		Quaternion newRot = Quaternion::EulerAnglesToQuaternion(0, targetAngle, 0);
		float finalRotSpeed = isOnGround ? rotationSpeed : rotationSpeed * (airControl * 0.5f);
		transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), newRot, finalRotSpeed * dt));

		Vector3 moveDir = newRot * Vector3(0, 0, -1.0f);
		physicsObject->AddForce(moveDir.Normalised() * (isOnGround ? moveSpeed : moveSpeed * airControl));
	}
	else if(physicsObject->GetLinearVelocity().Length() > 0.1f && isOnGround)
	{
		Vector3 dampedVel = Vector3::SmoothDamp(physicsObject->GetLinearVelocity(), Vector3(0, 0, 0), velocityRef, velocityDampSmoothness, FLT_MAX, dt);
		physicsObject->SetLinearVelocity(dampedVel);
	}

	Debug::Print("Score: " + std::to_string((int)score), Vector2(2, 5), Debug::WHITE);
	
	Vector4 healthColor = Debug::GREEN;
	if (currentHealth < 30.0f) healthColor = Debug::RED;
	if (currentHealth > 30.0f && currentHealth < 60.0f) healthColor = Debug::YELLOW;
	
	std::string healthStr = "Health: " + std::to_string((int)currentHealth);
	Debug::Print(healthStr, Vector2(89 - healthStr.length(), 10), healthColor);
}

void NCL::CSC8503::CWGoatPlayer::ResetPlayer(bool includeKey)
{
	currentHealth = maxHealth;

	physicsObject->SetLinearVelocity(Vector3(0, 0, 0));
	physicsObject->SetAngularVelocity(Vector3(0, 0, 0));

	transform.SetPosition(startPos);
	transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(startRot.x, startRot.y, startRot.z));

	score = 0.0f;

	ropePowerupCurrent = 0.0f;
	isHooked = false;
	ropePowerup = false;
	springRope->isEnabled = false;

	if (includeKey) hasDoorKey = false;
}