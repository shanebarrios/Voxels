#pragma once

#include "Utils/Coordinates.h"
#include "World/Block.h"
#include <optional>
#include <functional>

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