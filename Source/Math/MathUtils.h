#pragma once

#include <cmath>
#include <numbers>
#include "Utils/Coordinates.h"

namespace MathUtils
{
	inline constexpr float Lerp(float begin, float end, float alpha)
	{
		return begin + (end - begin) * alpha;
	}

	inline constexpr float DegsToRadians(float n)
	{
		return n * std::numbers::pi_v<float> / 180.0f;
	}

	inline int ManhattanDistance(ChunkCoords c1, ChunkCoords c2)
	{
		return std::abs(c1.X - c2.X) + std::abs(c1.Y - c2.Y) + std::abs(c1.Z - c2.Z);
	}

	inline int ManhattanDistance(BlockCoords b1, BlockCoords b2)
	{
		return std::abs(b1.X - b2.X) + std::abs(b1.Y - b2.Y) + std::abs(b1.Z - b2.Z);
	}
}