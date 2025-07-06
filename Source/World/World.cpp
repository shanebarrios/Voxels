#include "World.h"
#include <algorithm>
#include "../ECS/EntityFactory.h"
#include "../ECS/Components.h"
#include "../Physics/PhysicsSystem.h"
#include "../Window.h"
#include "../Math/MathUtils.h"
#include "../Logger.h"
#include "ChunkUtils.h"

// Through profiling, realized that rebuilding meshes was a huge bottleneck
// Realized that 4x mesh rebuilds for every chunk load
// Duplicated, so if I limit the amount of chunk remeshes and sort by distance to player (which chunk loads are already sorted by), 
// would vastly reduce redundant remeshes

constexpr int k_RenderDistance = 12;
int remeshes = 0;
int loaded = 0;

World::World()
{
	RegisterComponents();
	m_Player = EntityFactory::CreatePlayer(m_ECS, { 0.0f, 50.0f, 0.0f });
	m_PlayerController = std::make_unique<PlayerController>(m_Player, m_ECS, *this, PlayerController::ControlMode::Default);

	UpdateLoadedChunkQueue();
	LoadChunks();
	UpdateChunkMeshes();
	UpdateChunkRenderList();
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

Chunk* World::GetChunk(ChunkCoords chunkCoords)
{
	auto it = m_LoadedChunks.find(chunkCoords);
	if (it != m_LoadedChunks.end()) 
		return it->second.get();
	else 
		return nullptr;
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
	const ChunkCoords playerPositionOld = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);
	m_PlayerController->Update(input);
	PhysicsSystem::Update(m_ECS, *this);
	const ChunkCoords playerPositionNew = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);

	if (playerPositionOld != playerPositionNew)
	{
		UpdateLoadedChunkQueue();
		UnloadChunks();
		SortChunksByPlayerDistance();
		LOG_WARN("Entered new chunk!");
	}
	LoadChunks();
	UpdateChunkMeshes();
	UpdateChunkRenderList();
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
	const WorldCoords playerPosition = m_ECS.GetComponent<TransformComponent>(m_Player).Position;
	const ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(playerPosition);

	m_ChunkLoadList.clear();
	m_ChunkLoadIndex = 0;

	for (int y = -k_RenderDistance; y <= k_RenderDistance; y++)
	{
		for (int z = -k_RenderDistance; z <= k_RenderDistance; z++)
		{
			for (int x = -k_RenderDistance; x <= k_RenderDistance; x++)
			{
				ChunkCoords coords = playerChunkPosition + ChunkCoords{ x, y, z };
				if (m_LoadedChunks.find(coords) == m_LoadedChunks.end())
				{
					m_ChunkLoadList.push_back(coords);
				}
			}
		}
	}

	std::sort(m_ChunkLoadList.begin(), m_ChunkLoadList.end(), [playerChunkPosition](ChunkCoords c1, ChunkCoords c2)
		{
			ChunkCoords diff1 = c1 - playerChunkPosition;
			ChunkCoords diff2 = c2 - playerChunkPosition;
			return diff1.NormSq() < diff2.NormSq();
		});
}

void World::UnloadChunks()
{
	const WorldCoords playerPosition = m_ECS.GetComponent<TransformComponent>(m_Player).Position;
	const ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(playerPosition);

	for (auto it = m_LoadedChunks.begin(); it != m_LoadedChunks.end(); )
	{
		const ChunkCoords chunkPos = it->first;

		const ChunkCoords diff = chunkPos - playerChunkPosition;
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

void World::SortChunksByPlayerDistance()
{
	const ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);
	m_ChunksByDistance.clear();
	for (auto& [pos, chunk] : m_LoadedChunks)
	{
		m_ChunksByDistance.push_back(chunk.get());
	}
	std::sort(m_ChunksByDistance.begin(), m_ChunksByDistance.end(), [playerChunkPosition](const Chunk* c1, const Chunk* c2)
		{
			const ChunkCoords diff1 = c1->GetCoords() - playerChunkPosition;
			const ChunkCoords diff2 = c2->GetCoords() - playerChunkPosition;
			return diff1.NormSq() < diff2.NormSq();
		});
}

void World::UpdateChunkMeshes()
{
	int updated = 0;
	for (Chunk* chunk : m_ChunksByDistance)
	{
		if (chunk->NeedsRebuild())
		{
			remeshes++;
			updated++;
			chunk->RebuildMesh(*this);
		}
		if (updated > 64) break;
	}
}

void World::UpdateChunkRenderList()
{
	//const ChunkCoords playerChunkPosition = static_cast<ChunkCoords>(m_ECS.GetComponent<TransformComponent>(m_Player).Position);
	//m_ChunkRenderList.clear();
	//for (Chunk* chunk : m_ChunksByDistance)
	//{
	//	if (chunk->GetMesh().NumTransparentVertices() != 0)
	//	{
	//		m_ChunkRenderList.push_back(chunk.get());
	//	}
	//}
	//std::sort(m_ChunkRenderList.begin(), m_ChunkRenderList.end(), [playerChunkPosition](const Chunk* c1, const Chunk* c2)
	//	{
	//		const ChunkCoords diff1 = c1->GetCoords() - playerChunkPosition;
	//		const ChunkCoords diff2 = c2->GetCoords() - playerChunkPosition;
	//		return diff1.NormSq() > diff2.NormSq();
	//	});

	m_TransparentChunkRenderList.clear();
	m_OpaqueChunkRenderList.clear();

	for (const Chunk* chunk : m_ChunksByDistance)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumOpaqueVertices() > 0)
		{
			m_OpaqueChunkRenderList.push_back(chunk);
		}
		if (mesh.NumTransparentVertices() > 0)
		{
			m_TransparentChunkRenderList.push_back(chunk);
		}
	}
}
 
void World::LoadChunks()
{
	static constexpr int maxChunksLoaded = 256;
	for (int i = 0; i < maxChunksLoaded && m_ChunkLoadIndex < m_ChunkLoadList.size(); i++)
	{
		loaded++;
		const ChunkCoords coords = m_ChunkLoadList[m_ChunkLoadIndex++];
		
		for (BlockCoords faceNormal : ChunkUtils::k_FaceNormals)
		{
			const ChunkCoords neighbor{ coords.X + faceNormal.X, coords.Y + faceNormal.Y, coords.Z + faceNormal.Z };
			if (auto it = m_LoadedChunks.find(neighbor); it != m_LoadedChunks.end())
			{
				Chunk* neighbor = it->second.get();
				neighbor->TriggerRebuild();
			}
		}

		std::unique_ptr<Chunk> newChunk = m_WorldGenerator.GenerateChunk(coords);
		m_ChunksByDistance.push_back(newChunk.get());
		m_LoadedChunks[coords] = std::move(newChunk);
	}
}