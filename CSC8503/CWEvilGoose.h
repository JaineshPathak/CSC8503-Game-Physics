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

			bool LookingAtPlayer();

		protected:
			float meleeDamage = 2.0f;

			float pathTimer = 2.0f;
			float pathTimerCurrent = 0.0f;

			Vector3 mazePos;
			float mazeRadius = 160.0f;

			void FindRandomPatrolPoint();

			State* Idle;
			State* Roaming;
			State* Chasing;
		};
	}
}