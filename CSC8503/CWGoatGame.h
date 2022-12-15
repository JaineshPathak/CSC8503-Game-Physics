#pragma once
#include <iostream>

//Including these below gives loads of errors
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"

#include "CWLevelManager.h"
#include "CWGoatPlayer.h"
#include "CWFollowCamera.h"
#include "CWGrapplePowerup.h"
#include "CWDoorTrigger.h"
#include "CWPropDestroy.h"

#include "NavigationGrid.h"
#include "NavigationPath.h"

namespace NCL 
{
	namespace CSC8503
	{
		enum GameState
		{
			GameStarted = 0,
			GameEnded = 1
		};

		class CWGoatGame
		{
		public:
			CWGoatGame();
			~CWGoatGame();

			virtual void UpdateGame(float dt);

			bool GetCursorStatus() const { return toggleCamera; }
			CWGoatPlayer* GetPlayer() { return player; }

			GameState GetGameState() { return gameState; }
			void EndGame();

			void OnPropSpawn(CWPropDestroy* prop) 
			{ 
				totalPropsToDestroy++; 
				propsList.emplace_back(prop);
			}
			void OnPropDestroy(const int& propScore) 
			{ 
				currentPropsDestroyed++;
				if (currentPropsDestroyed >= totalPropsToDestroy)
					EndGame();
			}

			MeshGeometry* GetGoatMesh() { return levelManager->GetGoatMesh(); }
			MeshGeometry* GetDudeMesh() { return levelManager->GetDudeMesh(); }
			MeshGeometry* GetEnemyMesh() { return levelManager->GetEnemyMesh(); }

			ShaderBase* GetBasicShader() { return levelManager->GetBasicShader(); }
			TextureBase* GetWhiteTex() { return levelManager->GetWhiteTex(); }

			Vector3 GetRandomRoamPoint() { return levelManager->GetRandomRoamPoint(); }
			Vector3 GetRandomMazePoint() { return levelManager->GetRandomMazePoint(); }

			NavigationGrid* GetNavGrid() const { return navGrid; }
			void AddPawnToList(CWPawn* pawn) { pawnsList.emplace_back(pawn); }
			void AddPowerupToList(CWGrapplePowerup* powerUp) { powerupList.emplace_back(powerUp); }

		protected:
			GameState gameState;

			void InitCamera();

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;
			CWGoatPlayer* player;

			CWLevelManager* levelManager;

			Vector3 startCameraPos = Vector3(512.0f, 40.0f, 512.0f);
			Camera* cameraMain;
			CWFollowCamera* cameraFollow;

			std::vector<Vector3> testNodes;
			Vector3 startPathPos;
			Vector3 endPathPos;
			NavigationGrid* navGrid;
			NavigationPath navPath;
			void TestFindPath();
			void DisplayTestFindPath();
			float timer = 0.0f;

			bool useGravity = true;
			bool toggleCamera = false;
			bool toggleCursor = false;

			//Debug Mode
			//GameObject* selectedObject;

			int totalPropsToDestroy;
			int currentPropsDestroyed;

			std::vector<CWPawn*> pawnsList;
			std::vector<CWGrapplePowerup*> powerupList;
			std::vector<CWPropDestroy*> propsList;

			float gameTime = 300.0f;
			float gameTimeCurrent = 0.0f;

			std::string gameOver = std::string("Game Over");
			std::string yourScore = std::string("Your Score: ");
			std::string wantToPlayAgain = std::string("Do you want to play again?");
			std::string yes = std::string("Press Y to Replay");
			std::string no = std::string("Press N to Quit");
		};
	}
}