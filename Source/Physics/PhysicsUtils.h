#pragma once

#include "World/Coordinates.h"
#include "ECS/Components.h"

namespace PhysicsUtils
{
	inline BlockCoords ColliderBlocksMinBounds(const Collider& collider, WorldCoords position)
	{
		return static_cast<BlockCoords>(
			WorldCoords
			{
				position.X + collider.Min.X,
				position.Y + collider.Min.Y,
				position.Z + collider.Min.Z
			});
	}

	inline BlockCoords ColliderBlocksMaxBounds(const Collider& collider, WorldCoords position)
	{
		return static_cast<BlockCoords>(
			WorldCoords
			{
				position.X + collider.Max.X,
				position.Y + collider.Max.Y,
				position.Z + collider.Max.Z
			});
	}

	inline bool ColliderIntersectsBlock(BlockCoords blockCoords, const Collider& collider, WorldCoords position)
	{
		const BlockCoords min = ColliderBlocksMinBounds(collider, position);
		const BlockCoords max = ColliderBlocksMaxBounds(collider, position);
		if (blockCoords.X >= min.X && blockCoords.X <= max.X &&
			blockCoords.Y >= min.Y && blockCoords.Y <= max.Y &&
			blockCoords.Z >= min.Z && blockCoords.Z <= max.Z)
		{
			return true;
		}
		return false;
	}
}