#include "PhysicsSystem.h"

#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include "../ecs/ECS.h"
#include "../ecs/Components.h"
#include "../world/Block.h"
#include "../world/World.h"
#include "PhysicsUtils.h"

static constexpr size_t k_MaxCollidingBlocks = 16;

// Potentially could pass min and max into these functions? Prob wouldn't make much too much of a performance change tho

template <bool Positive>
static float CheckCollisionX(const World& world, const Collider& collider, WorldCoords position)
{
	const BlockCoords min = PhysicsUtils::ColliderBlocksMinBounds(collider, position);
	const BlockCoords max = PhysicsUtils::ColliderBlocksMaxBounds(collider, position);

	for (int y = min.Y; y <= max.Y; y++)
	{
		for (int z = min.Z; z <= max.Z; z++)
		{
			if (world.GetBlock({ Positive ? max.X : min.X, y, z }) != BlockType::Air)
			{
				return Positive ?
					static_cast<float>(max.X) - (position.X + collider.Max.X) - 0.001f :
					(static_cast<float>(min.X) + 1.0f) - (position.X + collider.Min.X) + 0.001f;

			}
		}
	}
	return 0.0f;
}



template <bool Positive>
static float CheckCollisionY(const World& world, const Collider& collider, WorldCoords position)
{
	const BlockCoords min = PhysicsUtils::ColliderBlocksMinBounds(collider, position);
	const BlockCoords max = PhysicsUtils::ColliderBlocksMaxBounds(collider, position);

	for (int x = min.X; x <= max.X; x++)
	{
		for (int z = min.Z; z <= max.Z; z++)
		{
			if (world.GetBlock({ x, Positive ? max.Y : min.Y, z }) != BlockType::Air)
			{
				return Positive ?
					static_cast<float>(max.Y) - (position.Y + collider.Max.Y) - 0.001f :
					(static_cast<float>(min.Y) + 1.0f) - (position.Y + collider.Min.Y) + 0.001f;
			}
		}
	}
	return 0.0f;
}

template <bool Positive>
static float CheckCollisionZ(const World& world, const Collider& collider, WorldCoords position)
{
	const BlockCoords min = PhysicsUtils::ColliderBlocksMinBounds(collider, position);
	const BlockCoords max = PhysicsUtils::ColliderBlocksMaxBounds(collider, position);

	for (int y = min.Y; y <= max.Y; y++)
	{
		for (int x = min.X; x <= max.X; x++)
		{
			if (world.GetBlock({ x, y, Positive ? max.Z : min.Z }) != BlockType::Air)
			{
				return Positive ?
					static_cast<float>(max.Z) - (position.Z + collider.Max.Z) - 0.001f :
					(static_cast<float>(min.Z) + 1.0f) - (position.Z + collider.Min.Z) + 0.001f;
			}
		}
	}
	return 0.0f;
}

static bool IsAirborne(const World& world, const Collider& collider, WorldCoords position)
{
	const WorldCoords translated = { position.X, position.Y - 0.002f, position.Z };
	return CheckCollisionY<false>(world, collider, translated) == 0.0f;
}

// Horizontal and vertical formulas shamelessly stolen from Minecraft parkour wiki

static void UpdateHorizontal(const World& world, PhysicsComponent& physics, TransformComponent& transform, const InputComponent* input)
{
	const float slipperiness = physics.Airborne ? 1.0f : 0.6f;
	if (input)
	{
		float movementMultiplier = 1.0f;
		if (input->InputFlags & InputComponent::Crouch)
		{
			movementMultiplier = 0.3f;
		}
		else if (input->InputFlags & InputComponent::Sprint)
		{
			movementMultiplier = 1.3f;
		}

		float acceleration = 0.0f;

		if (physics.Airborne)
		{
			acceleration = 0.02f * movementMultiplier;
		}
		else
		{
			acceleration = 0.1f * movementMultiplier * powf(0.6f / slipperiness, 3);
		}

		float normalizedX = input->MoveX;
		float normalizedZ = input->MoveZ;

		const float norm = sqrtf(powf(input->MoveX, 2) + powf(input->MoveZ, 2));

		if (norm > 0.0f)
		{
			normalizedX /= norm;
			normalizedZ /= norm;
		}

		physics.Velocity.X += normalizedX * acceleration;
		physics.Velocity.Z += normalizedZ * acceleration;
	}

	transform.Position.X += physics.Velocity.X;

	float collisionCorrection = 0.0f;
	if (physics.Velocity.X > 0.0f)
	{
		collisionCorrection = CheckCollisionX<true>(world, physics.Collider, transform.Position);
	}
	else if (physics.Velocity.X < 0.0f)
	{
		collisionCorrection = CheckCollisionX<false>(world, physics.Collider, transform.Position);
	}
	if (abs(collisionCorrection) > 0.0f)
	{
		transform.Position.X += collisionCorrection;
		physics.Velocity.X = 0.0f;
	}

	transform.Position.Z += physics.Velocity.Z;

	collisionCorrection = 0.0f;
	if (physics.Velocity.Z > 0.0f)
	{
		collisionCorrection = CheckCollisionZ<true>(world, physics.Collider, transform.Position);
	}
	else if (physics.Velocity.Z < 0.0f)
	{
		collisionCorrection = CheckCollisionZ<false>(world, physics.Collider, transform.Position);
	}
	if (std::abs(collisionCorrection) > 0.0f)
	{
		transform.Position.Z += collisionCorrection;
		physics.Velocity.Z = 0.0f;
	}

	physics.Velocity.X *= slipperiness * 0.91f;
	physics.Velocity.Z *= slipperiness * 0.91f;
}

static void UpdateVertical(const World& world, PhysicsComponent& physics, TransformComponent& transform, const InputComponent* input)
{
	if (input && (input->InputFlags & InputComponent::Jump) && !physics.Airborne)
	{
		physics.Velocity.Y = 0.42f;
	}
	
	// For cases where y velocity is > 1, have to check blocks in between for collisions. 
	// It is impossible for x and z velocity to be > 1 on the other hand, so this is only handled for y
	constexpr float step = 1.0f;
	const float sign = physics.Velocity.Y > 0.0f ? 1.0f : -1.0f;
	for (float remaining = physics.Velocity.Y * sign; remaining > 0.0f; remaining -= step)
	{
		transform.Position.Y += std::min(step, remaining) * sign;

		float collisionCorrection = 0.0f;
		if (sign > 0.0f)
		{
			collisionCorrection = CheckCollisionY<true>(world, physics.Collider, transform.Position);
		}
		else if (sign < 0.0f)
		{
			collisionCorrection = CheckCollisionY<false>(world, physics.Collider, transform.Position);
		}
		if (std::abs(collisionCorrection) > 0.0f)
		{
			physics.Velocity.Y = 0.0f;
			transform.Position.Y += collisionCorrection;
			break;
		}
	}

	physics.Velocity.Y = (physics.Velocity.Y - 0.08f) * 0.98f;
}


void PhysicsSystem::Update(ECS& ecs, World& world)
{
	ComponentArray<PhysicsComponent>& arr = ecs.GetComponentArray<PhysicsComponent>();
	for (auto [entity, physics] : arr)
	{
		TransformComponent& transform = ecs.GetComponent<TransformComponent>(entity);

		InputComponent* input = nullptr;

		if (ecs.HasComponent<InputComponent>(entity))
		{
			input = &ecs.GetComponent<InputComponent>(entity);
		}

		physics.Airborne = IsAirborne(world, physics.Collider, transform.Position);

		UpdateHorizontal(world, physics, transform, input);
		UpdateVertical(world, physics, transform, input);

		if (std::abs(physics.Velocity.X) < 0.005f) physics.Velocity.X = 0.0f;
		if (std::abs(physics.Velocity.Y) < 0.005f) physics.Velocity.Y = 0.0f;
		if (std::abs(physics.Velocity.Z) < 0.005f) physics.Velocity.Z = 0.0f;
	}
}
