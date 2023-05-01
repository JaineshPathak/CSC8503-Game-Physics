#pragma once
#include "CollisionVolume.h"
#include "Vector3.h"

namespace NCL {
	using namespace NCL::Maths;
	class AABBVolume : CollisionVolume
	{
	public:
		AABBVolume(const Vector3& halfDims) {
			type		= VolumeType::AABB;
			halfSizes	= halfDims;
			center = Maths::Vector3((halfSizes.x + halfSizes.x) * 0.5f,
				(halfSizes.y + halfSizes.y) * 0.5f,
				(halfSizes.z + halfSizes.z) * 0.5f);
		}

		Maths::Vector3 GetCenter() const { return center; }

		~AABBVolume() {

		}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

	protected:
		Vector3 halfSizes;
		Vector3 center;
	};
}
