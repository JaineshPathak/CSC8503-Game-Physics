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

			float scoreBonus = 10.0f;

			Vector3 basePos = Vector3(0, -6.5f, 0);
			float moveSpeed = 40.0f;
			float rotationSpeed = 5.0f;
			float distanceThreshold = 15.0f;
			Vector3 previousPosition;

			NavigationPath path;
			int currentDestinationIndex = 0;
			Vector3 currentDestination = Vector3(0, 0, 0);

			void MoveTowards(Vector3 src, const Vector3& pos, float dt);
			void MoveTowards(const Vector3& pos, float dt, bool useForce = false);
			void RotateTowards(const Vector3& pos, float rotSpeed, float dt);
			void RotateAway(const Vector3& pos, float rotSpeed, float dt);

			void FindPath(const Vector3& destination, std::vector<Vector3>& _pathList);
		};
	}
}