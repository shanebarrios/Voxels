#pragma once

#include "../ECS/ECS.h"
#include "../Memory/ChunkAllocator.h"
#include "Chunk.h"
#include "PlayerController.h"
#include "WorldGenerator.h"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

class Camera;
class UIOverlay;
struct TransformComponent;
struct LookComponent;

using ChunkMap = std::unordered_map<ChunkCoords, Chunk*>;

struct PlayerView
{
    const TransformComponent* Transform;
    const LookComponent* Look;
};

class World
{
  public:
    World() = default;

    void Init();

    void Update(const Camera& camera);

    BlockType GetBlock(BlockCoords blockCoords) const;
    Chunk* GetChunk(ChunkCoords chunkCoords);

    bool PlaceBlock(BlockType block, BlockCoords blockCoords);
    bool BreakBlock(BlockCoords blockCoords);

    PlayerView GetPlayerView() const;

    void SetPlayerActiveBlock(BlockType block);

    void SetPlayerPhysics(bool enabled);

    bool IsPlayerPhysicsEnabled() const;

    BlockType GetPlayerActiveBlock() const;

    void TogglePlayerController()
    {
        m_PlayerControllerEnabled = !m_PlayerControllerEnabled;
    }

    const std::vector<const Chunk*>& GetChunkRenderList() const
    {
        return m_ChunkRenderList;
    }

    const std::vector<const Chunk*>& GetChunkWaterRenderList() const
    {
        return m_WaterRenderList;
    }

    WorldCoords GetLightDir() const { return m_LightDir; }

    friend class UIOverlay;

  private:
    void RegisterComponents();

    void UpdateLoadedChunkQueue();
    void LoadChunks();
    void UnloadChunks();
    void SortChunksByPlayerDistance();
    void UpdateChunkMeshes();
    void UpdateChunkRenderList();

  private:
    ChunkMap m_LoadedChunks{};
    ECS m_ECS{};
    Entity m_Player{};

    std::unique_ptr<PlayerController> m_PlayerController{};
    std::vector<ChunkCoords> m_ChunkLoadList{};
    size_t m_ChunkLoadIndex = 0;
    std::vector<Chunk*> m_ChunksByDistance{};
    std::vector<const Chunk*> m_ChunkRenderList{};
    std::vector<const Chunk*> m_WaterRenderList{};
    WorldGenerator m_WorldGenerator{this};
    WorldCoords m_LightDir{0.6f, -0.7f, 0.2f};

    bool m_PlayerControllerEnabled = true;
};
