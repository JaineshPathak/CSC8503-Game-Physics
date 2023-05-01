#pragma once
#include "Constraint.h"
#include "GameObject.h"
#include "PhysicsObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWSpringConstraint : public Constraint
		{
		public:
			CWSpringConstraint(GameObject* a, Vector3 aP, float d, float sF, float sD);
			~CWSpringConstraint();

			void NewData(Vector3 aP, float d, float sF, float sD)
			{
				anchorPoint = aP;
				springForce = sF;
				springDamping = sD;
			}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			//GameObject* objectB;

			Vector3 anchorPoint;

			float distance;
			float springForce;
			float springDamping;
		};
	};
}