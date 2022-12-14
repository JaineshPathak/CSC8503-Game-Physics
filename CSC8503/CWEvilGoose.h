#pragma once
#include "CWPawn.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWEvilGoose : public CWPawn
		{
		public:
			CWEvilGoose(CWGoatGame& gGame, GameWorld& gWorld, const Vector3& pos,
				const Vector3& rot,
				const Vector3& scale,
				const float& radius,
				MeshGeometry* mesh,
				TextureBase* tex,
				ShaderBase* shader,
				const Vector4& color = Debug::RED);

			virtual void Update(float dt) override;
			virtual void OnCollisionBegin(GameObject* otherObject) override;
			void DebugDisplayPath(std::vector<Vector3> paths);

		protected:
			Vector3 basePos = Vector3(0, -6.5f, 0);
			float moveSpeed = 40.0f;
			float rotationSpeed = 30.0f;
			float distanceThreshold = 15.0f;
			Vector3 previousPosition;

			float chaseRadius = 45.0f;
			float chaseTimer = 5.0f;
			float chaseTimerCurrent = 0.0f;

			Vector3 mazePos;
			float mazeRadius = 120.0f;

			void MoveTowards(Vector3 src, const Vector3& pos, float dt);
			void MoveTowards(const Vector3& pos, float dt, bool useForce = false);
			void RotateTowards(const Vector3& pos, float rotSpeed, float dt);
			void RotateAway(const Vector3& pos, float rotSpeed, float dt);

			Vector3 currentRoamDestination = Vector3(0, 0, 0);

			State* Idle;
			State* Roaming;
			State* Chasing;
		};
	}
}