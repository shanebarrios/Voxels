#pragma once

#include "../Coordinates.h"
#include "../world/Block.h"
#include <optional>

class World;

class Raycast
{
public:
	struct RaycastHit
	{
		BlockCoords Coords;
		BlockFace Face;
	};

	Raycast(WorldCoords start, WorldCoords direction, float maxDistance);

	std::optional<RaycastHit> Cast(const World& world) const;

private:
	WorldCoords m_Start;
	WorldCoords m_Direction;
	float m_MaxDistance;
};