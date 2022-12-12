#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"
#include "State.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "CWGoatGame.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWPawn : public GameObject
		{
		public:
			CWPawn(CWGoatGame& gGame, GameWorld& gWorld) : goatGame(gGame), world(gWorld) { };
			virtual void Update(float dt) = 0;
			//virtual void DebugDisplayPath(std::vector<Vector3> paths);

		protected:
			CWGoatGame& goatGame;
			GameWorld& world;

			std::vector<Vector3> pathList;
			StateMachine* stateMachine;
		};
	}
}