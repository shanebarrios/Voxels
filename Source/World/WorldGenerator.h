#pragma once

#include "Utils/Coordinates.h"
#include <unordered_map>
#include <array>
#include <memory>
#include "Chunk.h"
#include "DataStructures/LRUCache.h"
#include "DataStructures/FixedBuffer.h"
#include "Math/Noise.h"

enum class TerrainFeature : uint8_t
{
	None = 0,
	Tree
};

enum class Biome : uint8_t
{
	Ocean,
	Beach,
	Forest,
	Mountains,
	SnowyMountains,
	Plains,
	Desert
};

using ChunkHeightMap = std::array<float, 16 * 16>;
using ChunkMoistureMap = std::array<float, 16 * 16>;

struct ChunkGenInfo
{
	ChunkHeightMap HeightMap;
	ChunkMoistureMap MoistureMap;
};

struct RelativeBlockPlacement
{
	BlockType Block;
	BlockCoords Coords;
};

struct LocalBlockPlacement
{
	BlockType Block;
	size_t Index;
};

class World;

class WorldGenerator
{
public:
	// Change this to use seed
	explicit WorldGenerator(World* world);

	std::unique_ptr<Chunk> GenerateChunk(ChunkCoords coords);

private:
	ChunkHeightMap GenerateHeightMap(ChunkCoords2D coords) const;

	ChunkMoistureMap GenerateMoistureMap(ChunkCoords2D coords) const;

	TerrainFeature GenerateTerrainFeature(ChunkCoords2D chunkCoords, LocalBlockCoords2D blockCoords, Biome biome) const;

	void BuildTerrainFeature(Chunk& chunk, LocalBlockCoords blockCoords, TerrainFeature terrainFeature);

	void BuildTerrainFeatures(Chunk& chunk, const ChunkGenInfo& genInfo);

	void BuildTerrain(Chunk& chunk, const ChunkGenInfo& genInfo) const;

	const ChunkGenInfo& GetChunkGenInfo(ChunkCoords2D coords);

	float GenerateHeight(int blockX, int blockZ) const;

	float GenerateMoisture(int blockX, int blockZ) const;

	Biome GetBiome(float height, float moisture) const;

	BlockType GetBlock(int surfaceHeight, int blockHeight, Biome biome) const;

private:
	World* m_World;
	LRUCache<ChunkCoords2D, ChunkGenInfo> m_Cache{1024};
	std::unordered_map<ChunkCoords, std::vector<LocalBlockPlacement>> m_ToPlace{};
	Noise::OctavePerlinNoise m_HeightOctaveNoise;
	Noise::OctavePerlinNoise m_MoistureOctaveNoise;
};