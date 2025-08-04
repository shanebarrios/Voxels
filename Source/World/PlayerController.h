#pragma once

#include "ECS/ECS.h"
#include "Utils/Coordinates.h"
#include "Block.h"
#include <optional>

struct InputComponent;
struct TransformComponent;
struct LookComponent;
struct PhysicsComponent;

class World;
class Camera;

class PlayerController
{
public:
	PlayerController(Entity player, ECS& ecs, World& world);

	void Update(const Camera& camera);

	void SetPhysicsEnabled(bool yes);

	BlockType GetActiveBlock() const { return m_ActiveBlock; }

	void SetActiveBlock(BlockType block) { m_ActiveBlock = block; }

private:
	void UpdateInput(const Camera& camera);
	void UpdateLook(const Camera& camera);
	void HandleBlockInteractions(const Camera& camera);
	void HandleDebugMovement();

private:
	ECS& m_ECS;
	Entity m_Player;
	World& m_World;
	TransformComponent& m_TransformComponent;
	LookComponent& m_LookComponent;
	InputComponent& m_InputComponent;
	PhysicsComponent* m_PhysicsComponent;

	BlockType m_ActiveBlock = BlockType::Stone;
};