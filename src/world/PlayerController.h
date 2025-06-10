#pragma once

#include "../ecs/ECS.h"
#include "../Coordinates.h"
#include "Block.h"
#include <optional>

struct InputComponent;
struct TransformComponent;
struct LookComponent;
struct PhysicsComponent;

struct GameInput;
class World;

class PlayerController
{
public:
	PlayerController(Entity player, ECS& ecs, World& world);

	void Update(const GameInput& input);

private:
	World& m_World;
	InputComponent& m_InputComponent;
	TransformComponent& m_TransformComponent;
	LookComponent& m_LookComponent;
	PhysicsComponent& m_PhysicsComponent;

	void UpdateInputComponent(const GameInput& input);
	void UpdateLook(const GameInput& input);
	void HandleBlockInteractions();
};