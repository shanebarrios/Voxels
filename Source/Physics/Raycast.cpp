#include "Raycast.h"
#include "../World/World.h"
#include <cmath>

// Algorithm adapted from
// http://www.cse.yorku.ca/~amana/research/grid.pdf

Raycast::Raycast(WorldCoords start, WorldCoords direction, float maxDistance) :
	m_Start{ start },
	m_Direction{ direction },
	m_MaxDistance{ maxDistance }
{

}

std::optional<Raycast::RaycastHit> Raycast::Cast(const World& world) const
{
	BlockCoords location = static_cast<BlockCoords>(m_Start);
	const BlockCoords step =
	{
		m_Direction.X > 0.0f ? 1 : -1,
		m_Direction.Y > 0.0f ? 1 : -1,
		m_Direction.Z > 0.0f ? 1 : -1
	};
	WorldCoords tMax =
	{
		(std::floor(m_Start.X) + (step.X > 0 ? 1.0f : 0.0f) - m_Start.X) / m_Direction.X,
		(std::floor(m_Start.Y) + (step.Y > 0 ? 1.0f : 0.0f) - m_Start.Y) / m_Direction.Y,
		(std::floor(m_Start.Z) + (step.Z > 0 ? 1.0f : 0.0f) - m_Start.Z) / m_Direction.Z
	};
	const WorldCoords tDelta =
	{
		std::abs(1.0f / m_Direction.X),
		std::abs(1.0f / m_Direction.Y),
		std::abs(1.0f / m_Direction.Z)
	};
	const float tEnd = m_MaxDistance / m_Direction.Length();

	while (true)
	{
		if (tMax.X <= tMax.Y && tMax.X <= tMax.Z)
		{
			if (tMax.X > tEnd) break;
			tMax.X += tDelta.X;
			location.X += step.X;
			if (IsInteractable(world.GetBlock(location)))
			{
				return Raycast::RaycastHit{ location, step.X > 0 ? BlockFace::NegX : BlockFace::PosX };
			}
		}
		else if (tMax.Y <= tMax.X && tMax.Y <= tMax.Z)
		{
			if (tMax.Y > tEnd) break;
			tMax.Y += tDelta.Y;
			location.Y += step.Y;
			if (IsInteractable(world.GetBlock(location)))
			{
				return Raycast::RaycastHit{ location, step.Y > 0 ? BlockFace::NegY : BlockFace::PosY };
			}
		}
		else
		{
			if (tMax.Z > tEnd) break;
			tMax.Z += tDelta.Z;
			location.Z += step.Z;
			if (IsInteractable(world.GetBlock(location)))
			{
				return Raycast::RaycastHit{ location, step.Z > 0 ? BlockFace::NegZ : BlockFace::PosZ };
			}
		}
	}
	return std::nullopt;
}