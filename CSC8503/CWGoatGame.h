#pragma once
#include <iostream>

//Including these below gives loads of errors
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"

#include "CWLevelManager.h"
#include "CWGoatPlayer.h"
#include "CWFollowCamera.h"

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

		protected:
			void InitCamera();

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;
			CWGoatPlayer* player;

			CWLevelManager* levelManager;
			CWFollowCamera* cameraFollow;

			bool useGravity = true;
		};
	}
}