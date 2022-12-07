#pragma once
#include "Camera.h"
#include "Transform.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWFollowCamera : public Camera
		{
		public:
			CWFollowCamera(Transform& target);
			~CWFollowCamera() {};

			virtual void UpdateCamera(float dt) override;

		protected:
			Transform& followTarget;
			Vector3 followOffset = Vector3(0, 20.0f, 30.0f);
			float pitchOffset = 12.0f;

			float h = 0.0f, v = 0.0f;
		};
	}
}