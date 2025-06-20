#include "WorldGenerator.h"
#include "../math/Noise.h"
#include "ChunkUtils.h"

// Prob should move this to config file
struct
{
	int NumOctives = 4;
	float Persistence = 0.5f;
	float Lacunarity = 2.0f;
	float BaseFrequency = 0.005f;
	float BaseAmplitude = 64.0f;
} OctavesConfig;

int WorldGenerator::GenerateHeight(int blockX, int blockZ) const
{
	float noiseAccum = 0.0f;
	float amplitude = OctavesConfig.BaseAmplitude;
	float frequency = OctavesConfig.BaseFrequency;
	for (int i = 0; i < OctavesConfig.NumOctives; i++)
	{
		const float x = blockX * frequency;
		const float z = blockZ * frequency;
		noiseAccum += Noise::PerlinNoise(x, z) * amplitude;

		frequency *= OctavesConfig.Lacunarity;
		amplitude *= OctavesConfig.Persistence;
	}
	return static_cast<int>(std::roundf(noiseAccum)) + 60;
}

ChunkHeightMap WorldGenerator::GenerateHeightMap(ChunkCoords2D coords) const
{
	ChunkHeightMap heightMap;
	for (uint8_t x = 0; x < 16; x++)
	{
		for (uint8_t z = 0; z < 16; z++)
		{
			const int height = GenerateHeight(coords.X * 16 + x, coords.Z * 16 + z);
			heightMap[ChunkUtils::PackXZ(x, z)] = height;
		}
	}
	return heightMap;
}

const ChunkHeightMap& WorldGenerator::GetChunkHeightMap(ChunkCoords2D coords)
{
	const ChunkHeightMap* heightMap = m_HeightMapCache.Get(coords);
	if (heightMap) return *heightMap;
	
	return m_HeightMapCache.Insert(coords, GenerateHeightMap(coords));
}

BlockType WorldGenerator::GetBlock(const ChunkHeightMap& heightMap, ChunkCoords chunkCoords, uint8_t xOffset, uint8_t yOffset, uint8_t zOffset) const
{
	if (chunkCoords.Y < -4) return BlockType::Air;

	const int height = heightMap[ChunkUtils::PackXZ(xOffset, zOffset)];
	const int blockY = chunkCoords.Y * 16 + yOffset;
	if (blockY > height)
	{
		return BlockType::Air;
	}
	else if (blockY == height)
	{
		return BlockType::Grass;
	}
	else if (blockY >= height - 2)
	{
		return BlockType::Dirt;
	}
	else
	{
		return BlockType::Stone;
	}
}

std::unique_ptr<Chunk> WorldGenerator::GenerateChunk(ChunkCoords coords)
{
	const ChunkHeightMap& heightMap = GetChunkHeightMap(static_cast<ChunkCoords2D>(coords));

	auto chunk = std::make_unique<Chunk>(coords);

	const BlockCoords start{ coords.X * 16, coords.Y * 16, coords.Z * 16 };

	for (uint8_t x = 0; x < 16; x++)
	{
		for (uint8_t z = 0; z < 16; z++)
		{
			for (uint8_t y = 0; y < 16; y++)
			{
				const BlockType block = GetBlock(heightMap, coords, x, y, z);
				chunk->SetBlock(block, x, y, z);
			}
		}
	}

	return chunk;
} 