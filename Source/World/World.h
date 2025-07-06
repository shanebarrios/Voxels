#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <array>
#include "Chunk.h"
#include <vector>
#include <queue>
#include "../ECS/ECS.h"
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
	Chunk* GetChunk(ChunkCoords chunkCoords);

	bool PlaceBlock(BlockType block, BlockCoords blockCoords);
	bool BreakBlock(BlockCoords blockCoords);

	PlayerView GetPlayerView() const;
	
	const std::vector<const Chunk*>& GetOpaqueChunkRenderList() const { return m_OpaqueChunkRenderList; }
	const std::vector<const Chunk*>& GetTransparentChunkRenderList() const { return m_TransparentChunkRenderList; }

private:
	ChunkMap m_LoadedChunks{};
	ECS m_ECS{};
	Entity m_Player{};  

	std::unique_ptr<PlayerController> m_PlayerController{};
	std::vector<ChunkCoords> m_ChunkLoadList{};
	size_t m_ChunkLoadIndex = 0;
	std::vector<Chunk*> m_ChunksByDistance{};
	std::vector<const Chunk*> m_OpaqueChunkRenderList{};
	std::vector<const Chunk*> m_TransparentChunkRenderList{};
	WorldGenerator m_WorldGenerator{this};
	
	void RegisterComponents();

	void UpdateLoadedChunkQueue();
	void LoadChunks();
	void UnloadChunks();
	void SortChunksByPlayerDistance();
	void UpdateChunkMeshes();
	void UpdateChunkRenderList();
};
