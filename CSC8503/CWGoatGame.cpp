#include "CWGoatGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::CWGoatGame::CWGoatGame()
{
	world = new GameWorld();
	mainCamera = world->GetMainCamera();
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 40, 60));

	renderer = new GameTechRenderer(*world);
	
	physics = new PhysicsSystem(*world);
	physics->UseGravity(useGravity);
	
	levelManager = new CWLevelManager(*world, *renderer);
	player = new CWGoatPlayer(*world, *renderer);
	
	cameraFollow = new CWFollowCamera(player->GetTransform());
	world->SetMainCamera(cameraFollow);

	useGravity = true;

	InitCamera();
}

NCL::CSC8503::CWGoatGame::~CWGoatGame()
{
	delete levelManager;
	delete player;
	delete cameraFollow;

	delete world;
	delete renderer;
	delete physics;
}

void NCL::CSC8503::CWGoatGame::UpdateGame(float dt)
{
	world->UpdateWorld(dt);
	world->GetMainCamera()->UpdateCamera(dt);

	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		toggleCamera = !toggleCamera;
		if (toggleCamera)
			world->SetMainCamera(mainCamera);
		else
			world->SetMainCamera(cameraFollow);;
	}

	if (player) player->Update(dt);
}

void NCL::CSC8503::CWGoatGame::InitCamera()
{
	if (world == nullptr || world->GetMainCamera() == nullptr)
		return;

	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 40, 60));
}