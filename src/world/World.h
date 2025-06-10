#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <queue>
#include "Chunk.h"
#include "../Coordinates.h"
#include "../ecs/ECS.h"
#include "PlayerController.h"

class Input;
struct TransformComponent;
struct LookComponent;

using ChunkMap = std::unordered_map<ChunkCoords, std::unique_ptr<Chunk>>;

struct GameInput
{
	const Input& Input;
	float LookYaw;
	float LookPitch;
};

struct PlayerView
{
	const TransformComponent* Transform;
	const LookComponent* Look;
};

class World
{
public:
	World();

	void Update(const GameInput& input);

	BlockType GetBlock(BlockCoords blockCoords) const;
	bool PlaceBlock(BlockType block, BlockCoords blockCoords);
	bool BreakBlock(BlockCoords blockCoords);

	PlayerView GetPlayerView() const;

	const ChunkMap& GetLoadedChunks() const { return m_LoadedChunks; }

private:
	ChunkMap m_LoadedChunks{};
	ECS m_ECS{};
	Entity m_Player{};  

	std::unique_ptr<PlayerController> m_PlayerController{};

	void RegisterComponents();

	void UpdateLoadedChunks();
	void UpdateUnloadedChunks();
	void UpdateChunkMeshes();
};
