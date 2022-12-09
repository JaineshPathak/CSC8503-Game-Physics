#pragma once
#include <iostream>

//Including these below gives loads of errors
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"

#include "CWLevelManager.h"
#include "CWGoatPlayer.h"
#include "CWFollowCamera.h"

#include "NavigationGrid.h"
#include "NavigationPath.h"

namespace NCL 
{
	namespace CSC8503
	{
		class CWGoatGame
		{
		public:
			CWGoatGame();
			~CWGoatGame();

			virtual void UpdateGame(float dt);

			bool GetCursorStatus() const { return toggleCamera; }
			CWGoatPlayer* GetPlayer() { return player; }

		protected:
			void InitCamera();

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;
			CWGoatPlayer* player;

			CWLevelManager* levelManager;

			Camera* mainCamera;
			Vector3 startCameraPos = Vector3(512.0f, 40.0f, 512.0f);

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
			GameObject* selectedObject;
		};
	}
}