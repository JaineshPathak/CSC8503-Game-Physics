#pragma once
#include "CWPawn.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWDude : public CWPawn
		{
		public:
			CWDude(CWGoatGame& gGame, GameWorld& gWorld, const Vector3& pos,
					const Vector3& rot, 
					const Vector3& scale,
					const float& radius,
					MeshGeometry* mesh,
					TextureBase* tex,
					ShaderBase* shader,
					const Vector4& color = Debug::BLUE);

			virtual void Update(float dt) override;
			virtual void OnCollisionBegin(GameObject* otherObject) override;
			void DebugDisplayPath(std::vector<Vector3> paths);

		protected:			

			float runningSpeed = 60.0f;
			float roamTimer = 8.0f;
			float roamTimerCurrent = 0.0f;

			float chaseRadius = 45.0f;
			float chaseTimer = 5.0f;
			float chaseTimerCurrent = 0.0f;

			void FindRandomPatrolPoint();

			State* Idle;
			State* Roaming;
			State* Running;
		};
	}
}