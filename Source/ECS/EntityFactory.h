#pragma once

#include <cstdint>
#include "ECS/Components.h"

using Entity = uint32_t;

struct WorldCoords;
class ECS;

namespace EntityFactory
{
	Entity CreatePlayer(ECS& ecs, const WorldCoords& coords);

	Entity CreateDebugPlayer(ECS& ecs, const WorldCoords& coords);

	PhysicsComponent CreatePlayerPhysicsComponent();
}