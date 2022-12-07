#include "CWGoatPlayer.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Maths.h"

NCL::CSC8503::CWGoatPlayer::CWGoatPlayer(GameWorld& gWorld, GameTechRenderer& gRenderer) : world(gWorld), renderer(gRenderer)
{
	goatMesh = renderer.LoadMesh("Goat.msh");
	whiteTex = renderer.LoadTexture("WhiteTex.png");
	basicShader = renderer.LoadShader("scene.vert", "scene.frag");

	AABBVolume* volume = new AABBVolume(startScale);
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

	world.AddGameObject(this);
}

NCL::CSC8503::CWGoatPlayer::~CWGoatPlayer()
{
	delete goatMesh;
	delete basicShader;
}

void NCL::CSC8503::CWGoatPlayer::Update(float dt)
{
	Vector3 linearMovement;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::W)) linearMovement.z = -1.0f;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::S)) linearMovement.z = 1.0f;

	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::A)) linearMovement.x = -1.0f;
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::D)) linearMovement.x = 1.0f;

	linearMovement.y = 0.0f;

	if (linearMovement.Length() > 0.1f)
	{
		float targetAngle = atan2(-linearMovement.x, -linearMovement.z);
		targetAngle = RadiansToDegrees(targetAngle) + world.GetMainCamera()->GetYaw();

		Quaternion newRot = Quaternion::EulerAnglesToQuaternion(0, targetAngle, 0);
		transform.SetOrientation(newRot);

		Vector3 moveDir = newRot * Vector3(0, 0, -1.0f);
		physicsObject->AddForce(moveDir.Normalised() * moveSpeed * dt);
	}
}