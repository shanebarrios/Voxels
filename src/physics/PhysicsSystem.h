#pragma once

#include <cstdint>

class ECS;
class World;

using Entity = uint32_t;

namespace PhysicsSystem
{
	void Update(ECS& ecs, World& world);
};