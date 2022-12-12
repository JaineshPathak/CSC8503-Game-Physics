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

		protected:
			float moveSpeed = 15.0f;
			float rotationSpeed = 0.3f;

			std::vector<Vector3> patrolPoints;
			int currentPatrolPointIndex = 0;
			int wpIndex = 0;
		};
	}
}