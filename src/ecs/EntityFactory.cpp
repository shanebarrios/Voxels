#include "EntityFactory.h"

#include "ECS.h"
#include "Components.h"
#include "../Coordinates.h"

Entity EntityFactory::CreatePlayer(ECS& ecs, const WorldCoords& coords)
{
	Entity player = ecs.CreateEntity();
	ecs.AddComponent<TransformComponent>(player, 
		{ 
			.Position = coords, 
			.Yaw = -90.0f, 
		});
	ecs.AddComponent<PhysicsComponent>(player,
		{
			.Velocity = WorldCoords {},
			.Collider =
			{
				.Min = WorldCoords {-0.3f, 0.0f, -0.3f},
				.Max = WorldCoords {0.3f, 1.8f, 0.3f}
			},
			.Airborne = false,
		});
	ecs.AddComponent<InputComponent>(player,
		{
			.MoveX = 0.0f,
			.MoveZ = 0.0f,
			.InputFlags = 0
		});
	ecs.AddComponent<LookComponent>(player,
		{
			.Offset = WorldCoords {0.0f, 1.6f, 0.0f},
			.Yaw = -90.0f,
			.Pitch = 0.0f
		});
	return player;
}