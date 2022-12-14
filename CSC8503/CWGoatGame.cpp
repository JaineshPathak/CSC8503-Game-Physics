#include "CWGoatGame.h"
#include "CWPawn.h"

#include "GameWorld.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::CWGoatGame::CWGoatGame()
{
	world = new GameWorld();
	cameraMain = world->GetMainCamera();
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

	navGrid = new NavigationGrid("CWNavGrid5.txt");
	//navGrid->DebugDraw(1);

	gameTimeCurrent = gameTime;

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
	world->GetMainCamera()->UpdateCamera(dt);
	
	for (size_t i = 0; i < pawnsList.size(); i++)
		pawnsList[i]->Update(dt);

	for (size_t i = 0; i < powerupList.size(); i++)
		powerupList[i]->Update(dt);
	
	if(player) player->Update(dt);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		toggleCamera = !toggleCamera;
		if (toggleCamera)
			world->SetMainCamera(cameraMain);
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
		//Debug::Print(mainCamera->GetPosition())
		//std::cout << mainCamera->GetPitch() << ", " << mainCamera->GetYaw() << std::endl;
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

	if (gameState == GameState::GameStarted)
	{
		Debug::Print("Items: " + std::to_string(currentPropsDestroyed) + " / " + std::to_string(totalPropsToDestroy), Vector2(2, 10), Debug::CYAN);

		gameTimeCurrent -= dt;
		int minutes = floor(gameTimeCurrent / 60.0f);
		int seconds = std::round(std::fmod(gameTimeCurrent, 60.0f));

		if (seconds == 60)
		{
			seconds = 0;
			minutes += 1;
		}

		Vector4 timerColor = gameTimeCurrent <= 20.0f ? Debug::RED : Debug::YELLOW;
		std::string time = "Time: " + std::to_string(minutes) + ":" + std::to_string(seconds);
		Debug::Print(time, Vector2(90 - time.length(), 5), timerColor);
		if (gameTimeCurrent <= 0.0f)
		{
			gameTimeCurrent = 0.0f;
			EndGame();
		}
	}
	else if (gameState == GameState::GameEnded)
	{
		Debug::Print(gameOver, Vector2(50 - gameOver.length(), 20), Debug::RED);
		Debug::Print(yourScore, Vector2(50 - yourScore.length(), 50), Debug::YELLOW);
		Debug::Print(wantToPlayAgain, Vector2(50 - wantToPlayAgain.length(), 70), Debug::WHITE);
		Debug::Print(yes, Vector2(50 - yes.length(), 80), Debug::CYAN);
		Debug::Print(no, Vector2(50 - no.length(), 85), Debug::RED);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::N)) exit(0);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Y))
		{
			InitCamera();

			gameTimeCurrent = gameTime;

			player->ResetPlayer();
			for (size_t i = 0; i < propsList.size(); i++)
				propsList[i]->ResetProp();

			currentPropsDestroyed = 0;
			gameState = GameState::GameStarted;
		}
	}
}

void NCL::CSC8503::CWGoatGame::EndGame()
{
	gameState = GameState::GameEnded;

	cameraMain->enableInput = false;
	cameraFollow->enableInput = false;

	world->SetMainCamera(cameraMain);
	world->GetMainCamera()->SetPosition(Vector3(0 + 512.0f, 280.0f, 500.0f + 512.0f));
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(0.0f);

	yourScore = "Your Score: " + std::to_string((int)player->GetScore());
}

void NCL::CSC8503::CWGoatGame::InitCamera()
{
	if (world == nullptr || world->GetMainCamera() == nullptr)
		return;

	world->SetMainCamera(cameraFollow);
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(startCameraPos);

	cameraMain->enableInput = true;
	cameraFollow->enableInput = true;
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
