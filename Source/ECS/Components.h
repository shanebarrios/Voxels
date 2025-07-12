#pragma once

#include "Utils/Coordinates.h"

struct TransformComponent
{
	WorldCoords Position;
	float Yaw;
};

struct Collider
{
	WorldCoords Min;
	WorldCoords Max;
};

struct PhysicsComponent
{
	WorldCoords Velocity;
	Collider Collider;
	bool Airborne;
};

struct InputComponent 
{
	float MoveX;
	float MoveZ;
	uint32_t InputFlags;

	enum InputFlag : uint32_t
	{
		Jump = 1u,
		Sprint = 1u << 1u,
		Crouch = 1u << 2u,
		Attack = 1u << 3u,
		Interact = 1u << 4u
	};
};

struct LookComponent
{
	WorldCoords Offset;
	float Yaw;
	float Pitch;
};