#include "PlayerController.h"

#include "../ECS/ECS.h"
#include "../ECS/Components.h"
#include "../Math/MathUtils.h"
#include "World.h"
#include "../Window.h"
#include "ChunkUtils.h"
#include "../Physics/Raycast.h"
#include "../Physics/PhysicsUtils.h"

PlayerController::PlayerController(Entity player, ECS& ecs, World& world, ControlMode controlMode) :
	m_TransformComponent{ ecs.GetComponent<TransformComponent>(player) },
	m_LookComponent{ ecs.GetComponent<LookComponent>(player) },
	m_InputComponent{ ecs.GetComponent<InputComponent>(player)},
	m_PhysicsComponent{ ecs.GetOptionalComponent<PhysicsComponent>(player) },
	m_World{ world },
	m_ControlMode {controlMode}
{

}

void PlayerController::Update(const GameInput& input)
{
	UpdateInput(input);
	UpdateLook(input);
	HandleBlockInteractions(input);
	if (m_ControlMode == ControlMode::Debug)
	{
		HandleDebugMovement();
	}
}

void PlayerController::UpdateLook(const GameInput& input)
{
	m_TransformComponent.Yaw = input.LookYaw;
	m_LookComponent.Pitch = input.LookPitch;
	m_LookComponent.Yaw = input.LookYaw;

	if (m_ControlMode == ControlMode::Default)
	{
		if (input.Input.IsPressed(Key::Ctrl))
		{
			m_LookComponent.Offset.Y = 1.3f;
			m_PhysicsComponent->Collider.Max.Y = 1.5f;
		}
		else
		{
			m_LookComponent.Offset.Y = 1.6f;
			m_PhysicsComponent->Collider.Max.Y = 1.8f;
		}
	}
}

void PlayerController::HandleBlockInteractions(const GameInput& input)
{
	if (!input.Input.IsPressed(Key::MouseLeft) && !input.Input.IsPressed(Key::MouseRight))
	{
		return;
	}

	const WorldCoords lookDirection =
	{
		std::cosf(MathUtils::DegsToRadians(m_LookComponent.Yaw)) * std::cosf(MathUtils::DegsToRadians(m_LookComponent.Pitch)),
		std::sinf(MathUtils::DegsToRadians(m_LookComponent.Pitch)),
		std::sinf(MathUtils::DegsToRadians(m_LookComponent.Yaw)) * std::cosf(MathUtils::DegsToRadians(m_LookComponent.Pitch))
	};
	const WorldCoords lookOrigin = m_TransformComponent.Position + m_LookComponent.Offset;

	const Raycast ray{ lookOrigin, lookDirection, 6.0f };
	const std::optional<Raycast::RaycastHit> result = ray.Cast(m_World);
	if (result)
	{
		if (input.Input.IsPressed(Key::MouseRight))
		{
			const BlockCoords normal = ChunkUtils::k_FaceNormals[static_cast<uint8_t>(result->Face)];
			const BlockCoords coords = result->Coords + normal;

			if (!m_PhysicsComponent || 
				!PhysicsUtils::ColliderIntersectsBlock(coords, m_PhysicsComponent->Collider, m_TransformComponent.Position))
			{
				m_World.PlaceBlock(BlockType::Stone, coords);
			}
		}
		else
		{
			m_World.BreakBlock(result->Coords);
		}
	}
}

void PlayerController::UpdateInput(const GameInput& input)
{
	uint32_t inputFlags = 0;
	if (input.Input.IsPressed(Key::Ctrl))
	{
		inputFlags |= InputComponent::Crouch;
	}
	if (input.Input.IsPressed(Key::MouseLeft))
	{
		inputFlags |= InputComponent::Attack;
	}
	if (input.Input.IsPressed(Key::MouseRight))
	{
		inputFlags |= InputComponent::Interact;
	}
	if (input.Input.IsPressed(Key::Space))
	{
		inputFlags |= InputComponent::Jump;
	}
	if (input.Input.IsPressed(Key::Shift))
	{
		inputFlags |= InputComponent::Sprint;
	}
	m_InputComponent.InputFlags = inputFlags;

	const float forwardX = cos(MathUtils::DegsToRadians(input.LookYaw));
	const float forwardZ = sin(MathUtils::DegsToRadians(input.LookYaw));

	const float rightX = -forwardZ;
	const float rightZ = forwardX;

	float moveX = 0.0f;
	float moveZ = 0.0f;

	if (input.Input.IsPressed(Key::W))
	{
		moveX += forwardX;
		moveZ += forwardZ;
	}
	if (input.Input.IsPressed(Key::S))
	{
		moveX -= forwardX;
		moveZ -= forwardZ;
	}
	if (input.Input.IsPressed(Key::A))
	{
		moveX -= rightX;
		moveZ -= rightZ;
	}
	if (input.Input.IsPressed(Key::D))
	{
		moveX += rightX;
		moveZ += rightZ;
	}

	const float norm = sqrtf(moveX * moveX + moveZ * moveZ);

	if (norm > 0.0f)
	{
		moveX /= norm;
		moveZ /= norm;
	}

	m_InputComponent.MoveX = moveX;
	m_InputComponent.MoveZ = moveZ;
}

void PlayerController::HandleDebugMovement()
{
	constexpr float moveXZPerTick = 1.0f;
	constexpr float moveYPerTick = 1.0f;
	if (m_InputComponent.InputFlags & InputComponent::Jump)
	{
		m_TransformComponent.Position.Y += moveYPerTick;
	}
	if (m_InputComponent.InputFlags & InputComponent::Crouch)
	{
		m_TransformComponent.Position.Y -= moveYPerTick;
	}
	m_TransformComponent.Position.X += m_InputComponent.MoveX * moveXZPerTick;
	m_TransformComponent.Position.Z += m_InputComponent.MoveZ * moveXZPerTick;
}