#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
			center		= Maths::Vector3((halfSizes.x + halfSizes.x) * 0.5f, 
										(halfSizes.y + halfSizes.y) * 0.5f,
										(halfSizes.z + halfSizes.z) * 0.5f);
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes * 0.5f;
		}

		Maths::Vector3 GetCenter() const { return center; }

	protected:
		Maths::Vector3 halfSizes;
		Maths::Vector3 center;
	};
}

