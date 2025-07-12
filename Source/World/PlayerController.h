#pragma once

#include "ECS/ECS.h"
#include "Utils/Coordinates.h"
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
	enum class ControlMode
	{
		Default,
		Debug
	};

	PlayerController(Entity player, ECS& ecs, World& world, ControlMode controlMode = ControlMode::Default);

	void Update(const GameInput& gameInput);

protected:
	World& m_World;
	TransformComponent& m_TransformComponent;
	LookComponent& m_LookComponent;
	InputComponent& m_InputComponent;
	PhysicsComponent* m_PhysicsComponent;
	ControlMode m_ControlMode;

	void UpdateInput(const GameInput& input);
	void UpdateLook(const GameInput& input);
	void HandleBlockInteractions(const GameInput& input);
	void HandleDebugMovement();
};

//class DefaultPlayerController : public BasePlayerController
//{
//public:
//	DefaultPlayerController(Entity player, ECS& ecs, World& world);
//private:
//	PhysicsComponent& m_PhysicsComponent;
//
//	void HandleMovement(const GameInput& input) override;
//	void UpdateLook(const GameInput& input) override;
//	bool CanPlaceBlock(BlockCoords blockCoords) override;
//};
//
//class DebugPlayerController : public BasePlayerController
//{
//public:
//	DebugPlayerController(Entity player, ECS& ecs, World& world);
//private:
//	void HandleMovement(const GameInput& input) override;
//};