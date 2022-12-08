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

			bool GetCursorStatus() const { return toggleCursor; }

		protected:
			void InitCamera();

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;
			CWGoatPlayer* player;

			CWLevelManager* levelManager;

			Camera* mainCamera;
			CWFollowCamera* cameraFollow;

			NavigationGrid* navGrid;

			bool useGravity = true;
			bool toggleCamera = false;
			bool toggleCursor = false;

			//Debug Mode
			GameObject* selectedObject;
		};
	}
}