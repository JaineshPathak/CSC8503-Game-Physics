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
	world->GetMainCamera()->SetPosition(startCameraPos);

	renderer = new GameTechRenderer(*world);
	
	physics = new PhysicsSystem(*world);
	physics->UseGravity(useGravity);
	
	levelManager = new CWLevelManager(*world, *this, *renderer);
	player = new CWGoatPlayer(*this, *world, *renderer);
	
	cameraFollow = new CWFollowCamera(*world, *player);
	world->SetMainCamera(cameraFollow);

	navGrid = new NavigationGrid("CWNavGrid.txt");
	//navGrid->DebugDraw(1);

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
	if(player) player->Update(dt);
	world->GetMainCamera()->UpdateCamera(dt);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		toggleCamera = !toggleCamera;
		if (toggleCamera)
			world->SetMainCamera(mainCamera);
		else
		{
			world->SetMainCamera(cameraFollow);
			toggleCursor = false;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	if (toggleCamera)
	{
		if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
		{
			toggleCursor = !toggleCursor;
			if (toggleCursor)
			{
				Window::GetWindow()->ShowOSPointer(true);
				Window::GetWindow()->LockMouseToWindow(false);
			}
			else
			{
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
			}
		}
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void NCL::CSC8503::CWGoatGame::InitCamera()
{
	if (world == nullptr || world->GetMainCamera() == nullptr)
		return;

	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(startCameraPos);
}

void NCL::CSC8503::CWGoatGame::TestFindPath()
{
	startPathPos = Vector3(512.0f, 2.5, 768.0f);
	Debug::DrawBox(startPathPos, Vector3(1, 1, 1), Debug::RED, 1000.0f);

	endPathPos = Vector3(player->GetTransform().GetPosition().x, 2.5f, player->GetTransform().GetPosition().z);
	Debug::DrawBox(endPathPos, Vector3(1, 1, 1), Debug::BLUE);
	
	NavigationPath path;
	bool found = navGrid->FindPath(startPathPos, endPathPos, path);

	if (found)
	{
		testNodes.clear();
		Vector3 pos;
		while (path.PopWaypoint(pos))
			testNodes.push_back(pos);

		DisplayTestFindPath();
	}
}

void NCL::CSC8503::CWGoatGame::DisplayTestFindPath()
{
	for (int i = 1; i < testNodes.size(); ++i)
	{
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Debug::GREEN);
	}
}
