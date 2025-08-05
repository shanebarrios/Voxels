#include "PlayerController.h"

#include "ECS/ECS.h"
#include "ECS/Components.h"
#include "ECS/EntityFactory.h"
#include "Math/MathUtils.h"
#include "World.h"
#include "Rendering/Camera.h"
#include "ChunkUtils.h"
#include "Physics/Raycast.h"
#include "Physics/PhysicsUtils.h"
#include "Input.h"
#include "Utils/Logger.h"

PlayerController::PlayerController(Entity player, ECS& ecs, World& world) :
	m_ECS{ ecs },
	m_Player { player },
	m_TransformComponent{ ecs.GetComponent<TransformComponent>(player) },
	m_LookComponent{ ecs.GetComponent<LookComponent>(player) },
	m_InputComponent{ ecs.GetComponent<InputComponent>(player)},
	m_PhysicsComponent{ ecs.GetOptionalComponent<PhysicsComponent>(player) },
	m_World{ world }
{

}

void PlayerController::Update(const Camera& camera)
{
	UpdateInput(camera);
	UpdateLook(camera);
	HandleBlockInteractions(camera);
	if (!m_PhysicsComponent)
	{
		HandleDebugMovement();
	}
}

void PlayerController::SetPhysicsEnabled(bool yes)
{
	if (yes && !m_PhysicsComponent)
	{
		m_PhysicsComponent = &m_ECS.AddComponent<PhysicsComponent>(m_Player, EntityFactory::CreatePlayerPhysicsComponent());
	}
	else if (!yes && m_PhysicsComponent)
	{
		m_ECS.RemoveComponent<PhysicsComponent>(m_Player);
		m_PhysicsComponent = nullptr;
	}
}

void PlayerController::UpdateLook(const Camera& camera)
{
	m_TransformComponent.Yaw = camera.GetYaw();
	m_LookComponent.Pitch = camera.GetPitch();
	m_LookComponent.Yaw = camera.GetYaw();

	if (m_PhysicsComponent)
	{
		if (Input::IsPressed(KeyCode::Ctrl))
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

void PlayerController::HandleBlockInteractions(const Camera& camera)
{
	if (!Input::IsPressed(KeyCode::MouseLeft) && 
		!Input::IsPressed(KeyCode::MouseRight))
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
		if (Input::IsPressed(KeyCode::MouseRight))
		{
			const BlockCoords normal = ChunkUtils::k_FaceNormals[static_cast<uint8_t>(result->Face)];
			const BlockCoords coords = result->Coords + normal;

			if (!m_PhysicsComponent || 
				!PhysicsUtils::ColliderIntersectsBlock(coords, m_PhysicsComponent->Collider, m_TransformComponent.Position))
			{
				m_World.PlaceBlock(m_ActiveBlock, coords);
			}
		}
		else
		{
			m_World.BreakBlock(result->Coords);
		}
	}
}

void PlayerController::UpdateInput(const Camera& camera)
{
	uint32_t inputFlags = 0;
	if (Input::IsPressed(KeyCode::Ctrl))
	{
		inputFlags |= InputComponent::Crouch;
	}
	if (Input::IsPressed(KeyCode::MouseLeft))
	{
		inputFlags |= InputComponent::Attack;
	}
	if (Input::IsPressed(KeyCode::MouseRight))
	{
		inputFlags |= InputComponent::Interact;
	}
	if (Input::IsPressed(KeyCode::Space))
	{
		inputFlags |= InputComponent::Jump;
	}
	if (Input::IsPressed(KeyCode::Shift))
	{
		inputFlags |= InputComponent::Sprint;
	}
	m_InputComponent.InputFlags = inputFlags;

	const float forwardX = cos(MathUtils::DegsToRadians(camera.GetYaw()));
	const float forwardZ = sin(MathUtils::DegsToRadians(camera.GetYaw()));

	const float rightX = -forwardZ;
	const float rightZ = forwardX;

	float moveX = 0.0f;
	float moveZ = 0.0f;

	if (Input::IsPressed(KeyCode::W))
	{
		moveX += forwardX;
		moveZ += forwardZ;
	}
	if (Input::IsPressed(KeyCode::S))
	{
		moveX -= forwardX;
		moveZ -= forwardZ;
	}
	if (Input::IsPressed(KeyCode::A))
	{
		moveX -= rightX;
		moveZ -= rightZ;
	}
	if (Input::IsPressed(KeyCode::D))
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