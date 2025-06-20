#pragma once

#include "../Coordinates.h"
#include <unordered_map>
#include <array>
#include <memory>
#include "Chunk.h"
#include "../LRUCache.h"

using ChunkHeightMap = std::array<int, 16 * 16>;

class WorldGenerator
{
public:
	// Change this to use seed
	WorldGenerator() = default;

	std::unique_ptr<Chunk> GenerateChunk(ChunkCoords coords);
private:
	LRUCache<ChunkCoords2D, ChunkHeightMap> m_HeightMapCache{4096};

	ChunkHeightMap GenerateHeightMap(ChunkCoords2D coords) const;

	const ChunkHeightMap& GetChunkHeightMap(ChunkCoords2D coords);

	int GenerateHeight(int blockX, int blockZ) const;

	BlockType GetBlock(const ChunkHeightMap& heightMap, ChunkCoords chunkCoords, uint8_t xOffset, uint8_t yOffset, uint8_t zOffset) const;
};