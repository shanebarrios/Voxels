#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <array>
#include "Chunk.h"
#include <queue>
#include "../ecs/ECS.h"
#include "PlayerController.h"
#include "WorldGenerator.h"

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

constexpr size_t k_MaxRenderDistance = 12;

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

	struct ChunkLoadQueue
	{
		std::vector<ChunkCoords> Data{};
		size_t Index = 0;
	};

	std::unique_ptr<PlayerController> m_PlayerController{};
	ChunkLoadQueue m_ChunkLoadQueue{};
	WorldGenerator m_WorldGenerator{};
	
	void RegisterComponents();

	void UpdateLoadedChunkQueue();
	void LoadChunks();
	void UnloadChunks();
	void UpdateChunkMeshes();
};
