#include "PlayerController.h"

#include "../ecs/ECS.h"
#include "../ecs/Components.h"
#include "../math/MathUtils.h"
#include "World.h"
#include "../Window.h"
#include "ChunkUtils.h"
#include "../physics/Raycast.h"
#include "../physics/PhysicsUtils.h"

PlayerController::PlayerController(Entity player, ECS& ecs, World& world) :
	m_TransformComponent{ ecs.GetComponent<TransformComponent>(player) },
	m_LookComponent{ ecs.GetComponent<LookComponent>(player) },
	m_PhysicsComponent{ ecs.GetComponent<PhysicsComponent>(player) },
	m_InputComponent{ ecs.GetComponent<InputComponent>(player) },
	m_World{ world }
{

}


void PlayerController::Update(const GameInput& input)
{
	UpdateInputComponent(input);
	UpdateLook(input);
	HandleBlockInteractions();
}

void PlayerController::UpdateInputComponent(const GameInput& input)
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

	m_InputComponent.MoveX = 0;
	m_InputComponent.MoveZ = 0;

	const float forwardX = cos(MathUtils::DegsToRadians(input.LookYaw));
	const float forwardZ = sin(MathUtils::DegsToRadians(input.LookYaw));

	const float rightX = -sin(MathUtils::DegsToRadians(input.LookYaw));
	const float rightZ = cos(MathUtils::DegsToRadians(input.LookYaw));

	if (input.Input.IsPressed(Key::W))
	{
		m_InputComponent.MoveX += forwardX;
		m_InputComponent.MoveZ += forwardZ;
	}
	if (input.Input.IsPressed(Key::S))
	{
		m_InputComponent.MoveX -= forwardX;
		m_InputComponent.MoveZ -= forwardZ;
	}
	if (input.Input.IsPressed(Key::A))
	{
		m_InputComponent.MoveX -= rightX;
		m_InputComponent.MoveZ -= rightZ;
	}
	if (input.Input.IsPressed(Key::D))
	{
		m_InputComponent.MoveX += rightX;
		m_InputComponent.MoveZ += rightZ;
	}
}

void PlayerController::UpdateLook(const GameInput& input)
{
	m_TransformComponent.Yaw = input.LookYaw;
	m_LookComponent.Pitch = input.LookPitch;
	m_LookComponent.Yaw = input.LookYaw;


	if (input.Input.IsPressed(Key::Ctrl))
	{
		m_LookComponent.Offset.Y = 1.3f;
		m_PhysicsComponent.Collider.Max.Y = 1.5f;
	}
	else
	{
		m_LookComponent.Offset.Y = 1.6f;
		m_PhysicsComponent.Collider.Max.Y = 1.8f;
	}
}

void PlayerController::HandleBlockInteractions()
{
	if (!(m_InputComponent.InputFlags & (InputComponent::Interact | InputComponent::Attack)))
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
		if (m_InputComponent.InputFlags & InputComponent::Interact)
		{
			const std::array<int, 3>& normal = ChunkUtils::k_FaceNormals[static_cast<uint8_t>(result->Face)];
			const BlockCoords coords =
			{
				result->Coords.X + normal[0],
				result->Coords.Y + normal[1],
				result->Coords.Z + normal[2]
			};

			if (!PhysicsUtils::ColliderIntersectsBlock(coords, m_PhysicsComponent.Collider, m_TransformComponent.Position))
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