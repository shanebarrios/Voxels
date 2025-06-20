#include "World.h"
#include <algorithm>
#include "../ecs/EntityFactory.h"
#include "../ecs/Components.h"
#include "../physics/PhysicsSystem.h"
#include "../Window.h"
#include "../math/MathUtils.h"
#include "../Logger.h"
#include "ChunkUtils.h"

constexpr int k_RenderDistance = 12;

World::World()
{
	RegisterComponents();
	m_Player = EntityFactory::CreatePlayer(m_ECS, { 8.0f, 100.0f, 8.0f });
	m_PlayerController = std::make_unique<PlayerController>(m_Player, m_ECS, *this);

	UpdateLoadedChunkQueue();
	LoadChunks();
	UpdateChunkMeshes();
	PhysicsSystem::Update(m_ECS, *this);
}

BlockType World::GetBlock(BlockCoords blockCoords) const
{
	const ChunkCoords chunkCoords = static_cast<ChunkCoords>(blockCoords);
	auto it = m_LoadedChunks.find(chunkCoords);
	if (it != m_LoadedChunks.end())
	{

		const uint8_t x = static_cast<uint8_t>(blockCoords.X - 16 * chunkCoords.X);
		const uint8_t y = static_cast<uint8_t>(blockCoords.Y - 16 * chunkCoords.Y);
		const uint8_t z = static_cast<uint8_t>(blockCoords.Z - 16 * chunkCoords.Z);
		return it->second->GetBlock(x, y, z);
	}
	else
	{
		return BlockType::Air;
	}
}

bool World::PlaceBlock(BlockType block, BlockCoords blockCoords)
{
	const ChunkCoords chunkCoords = static_cast<ChunkCoords>(blockCoords);
	auto it = m_LoadedChunks.find(chunkCoords);
	if (it != m_LoadedChunks.end())
	{
		const uint8_t x = static_cast<uint8_t>(blockCoords.X - 16 * chunkCoords.X);
		const uint8_t y = static_cast<uint8_t>(blockCoords.Y - 16 * chunkCoords.Y);
		const uint8_t z = static_cast<uint8_t>(blockCoords.Z - 16 * chunkCoords.Z);
		it->second->SetBlock(block, x, y, z);

		std::array<ChunkCoords, 3> buf;
		const size_t neighborCount = ChunkUtils::NeighboringChunks(chunkCoords, x, y, z, buf);
		for (size_t i = 0; i < neighborCount; i++)
		{
			if (it = m_LoadedChunks.find(buf[i]); it != m_LoadedChunks.end())
			{
				it->second->TriggerRebuild();
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool World::BreakBlock(BlockCoords blockCoords)
{
	return PlaceBlock(BlockType::Air, blockCoords);
}

PlayerView World::GetPlayerView() const
{
	return
	{
		&m_ECS.GetComponent<TransformComponent>(m_Player),
		&m_ECS.GetComponent<LookComponent>(m_Player)
	};
}

void World::Update(const GameInput& input)
{
	m_PlayerController->Update(input);

	ChunkCoords playerPositionOld = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);
	PhysicsSystem::Update(m_ECS, *this);
	ChunkCoords playerPositionNew = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);

	if (playerPositionOld != playerPositionNew)
	{
		UpdateLoadedChunkQueue();
		UnloadChunks();
	}
	LoadChunks();
	UpdateChunkMeshes();
}

void World::RegisterComponents()
{
	m_ECS.RegisterComponent<TransformComponent>();
	m_ECS.RegisterComponent<PhysicsComponent>();
	m_ECS.RegisterComponent<InputComponent>();
	m_ECS.RegisterComponent<LookComponent>();
}

void World::UpdateLoadedChunkQueue()
{
	WorldCoords playerPosition = m_ECS.GetComponent<TransformComponent>(m_Player).Position;
	ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(playerPosition);

	std::vector<ChunkCoords> chunkLoadList{};

	for (int y = -k_RenderDistance; y <= k_RenderDistance; y++)
	{
		for (int z = -k_RenderDistance; z <= k_RenderDistance; z++)
		{
			for (int x = -k_RenderDistance; x <= k_RenderDistance; x++)
			{
				ChunkCoords coords = playerChunkPosition + ChunkCoords{ x, y, z };
				if (m_LoadedChunks.find(coords) == m_LoadedChunks.end())
				{
					chunkLoadList.push_back(coords);
				}
			}
		}
	}

	std::sort(chunkLoadList.begin(), chunkLoadList.end(), [playerChunkPosition](ChunkCoords c1, ChunkCoords c2)
		{
			ChunkCoords diff1 = c1 - playerChunkPosition;
			ChunkCoords diff2 = c2 - playerChunkPosition;
			return diff1.NormSq() < diff2.NormSq();
		});

	m_ChunkLoadQueue.Data = std::move(chunkLoadList);
	m_ChunkLoadQueue.Index = 0;
}

void World::UnloadChunks()
{
	WorldCoords playerPosition = m_ECS.GetComponent<TransformComponent>(m_Player).Position;
	ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(playerPosition);

	for (auto it = m_LoadedChunks.begin(); it != m_LoadedChunks.end(); )
	{
		ChunkCoords chunkPos = it->first;

		ChunkCoords diff = chunkPos - playerChunkPosition;
		if (std::abs(diff.X) > k_RenderDistance || std::abs(diff.Y) > k_RenderDistance || std::abs(diff.Z) > k_RenderDistance)
		{
			it = m_LoadedChunks.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void World::UpdateChunkMeshes()
{
	for (auto& [chunkPos, chunk] : m_LoadedChunks)
	{
		if (chunk->NeedsRebuild())
		{
			chunk->RebuildMesh(*this);
		}
	}
}

void World::LoadChunks()
{
	static constexpr int maxChunksLoaded = 64;
	for (int i = 0; i < maxChunksLoaded && m_ChunkLoadQueue.Index < m_ChunkLoadQueue.Data.size(); i++)
	{
		ChunkCoords coords = m_ChunkLoadQueue.Data[m_ChunkLoadQueue.Index];
		m_ChunkLoadQueue.Index++;
		m_LoadedChunks[coords] = m_WorldGenerator.GenerateChunk(coords);
	}
}