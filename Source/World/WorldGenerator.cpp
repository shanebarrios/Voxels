#include "WorldGenerator.h"
#include "Math/Noise.h"
#include "ChunkUtils.h"
#include <span>
#include <cassert>
#include "Core/Logger.h"
#include "World.h"

// Prob should move this to separate config file, and pass in somehow
struct
{
	Noise::OctaveConfig OctaveConfig
	{
		.NumOctaves = 4,
		.Persistence = 0.5f,
		.Lacunarity = 2.0f,
		.BaseFrequency = 0.005f
	};

	float Exponent = 1.0f;
	int BaseHeight = 0;
	int Amplitude = 100;
} HeightMapConfig;

struct
{
	Noise::OctaveConfig OctaveConfig
	{
		.NumOctaves = 2,
		.Persistence = 0.5f,
		.Lacunarity = 2.0f,
		.BaseFrequency = 0.005f
	};
	float Offset = 1000.0f;
} MoistureMapConfig;

struct
{
	float Frequency = 0.5f;
	float ForestCutOff = 0.65f;
	float PlainsCutoff = 0.8f;
} TerrainFeaturesConfig;

static std::span<RelativeBlockPlacement> GetFeatureBlocks(TerrainFeature feature)
{
	static RelativeBlockPlacement treeBlocks[]
	{
		{BlockType::Log, {0, 0, 0}},
		{BlockType::Log, {0, 1, 0}},
		{BlockType::Log, {0, 2, 0}},
		{BlockType::Log, {0, 3, 0}},
		{BlockType::Leaves, {0, 4, 0}},

		{BlockType::Leaves, {-1, 2, -1}},
		{BlockType::Leaves, {-1, 2, 0}},
		{BlockType::Leaves, {-1, 2, 1}},
		{BlockType::Leaves, {0, 2, -1}},
		{BlockType::Leaves, {0, 2, 1}},
		{BlockType::Leaves, {1, 2, -1}},
		{BlockType::Leaves, {1, 2, 0}},
		{BlockType::Leaves, {1, 2, 1}},

		{BlockType::Leaves, {-2, 2, -1}},
		{BlockType::Leaves, {-2, 2, 0}},
		{BlockType::Leaves, {-2, 2, 1}},
		{BlockType::Leaves, {2, 2, -1}},
		{BlockType::Leaves, {2, 2, 0}},
		{BlockType::Leaves, {2, 2, 1}},
		{BlockType::Leaves, {-1, 2, 2}},
		{BlockType::Leaves, {0, 2, 2}},
		{BlockType::Leaves, {1, 2, 2}},
		{BlockType::Leaves, {-1, 2, -2}},
		{BlockType::Leaves, {0, 2, -2}},
		{BlockType::Leaves, {1, 2, -2}},

		{BlockType::Leaves, {-1, 3, -1}},
		{BlockType::Leaves, {-1, 3, 0}},
		{BlockType::Leaves, {-1, 3, 1}},
		{BlockType::Leaves, {0, 3, -1}},
		{BlockType::Leaves, {0, 3, 1}},
		{BlockType::Leaves, {1, 3, -1}},
		{BlockType::Leaves, {1, 3, 0}},
		{BlockType::Leaves, {1, 3, 1}},

		{BlockType::Leaves, {-2, 3, -1}},
		{BlockType::Leaves, {-2, 3, 0}},
		{BlockType::Leaves, {-2, 3, 1}},
		{BlockType::Leaves, {2, 3, -1}},
		{BlockType::Leaves, {2, 3, 0}},
		{BlockType::Leaves, {2, 3, 1}},
		{BlockType::Leaves, {-1, 3, 2}},
		{BlockType::Leaves, {0, 3, 2}},
		{BlockType::Leaves, {1, 3, 2}},
		{BlockType::Leaves, {-1, 3, -2}},
		{BlockType::Leaves, {0, 3, -2}},
		{BlockType::Leaves, {1, 3, -2}},

		{BlockType::Leaves, {-1, 4, -1}},
		{BlockType::Leaves, {-1, 4, 0}},
		{BlockType::Leaves, {-1, 4, 1}},
		{BlockType::Leaves, {0, 4, -1}},
		{BlockType::Leaves, {0, 4, 1}},
		{BlockType::Leaves, {1, 4, -1}},
		{BlockType::Leaves, {1, 4, 0}},
		{BlockType::Leaves, {1, 4, 1}},

		{BlockType::Leaves, {0, 5, 0}}
	};
	static std::array<std::span<RelativeBlockPlacement>, 1> featureToBlocks
	{
		treeBlocks
	};
	assert(feature != TerrainFeature::None);
	return featureToBlocks[static_cast<uint8_t>(feature) - 1];
}

static int BlockHeightFromFloat(float floatHeight)
{
	const float blockHeightF = floatHeight * HeightMapConfig.Amplitude + HeightMapConfig.BaseHeight;
	return static_cast<int>(std::roundf(blockHeightF));
}

WorldGenerator::WorldGenerator(World* world) :
	m_World {world},
	m_HeightOctaveNoise {HeightMapConfig.OctaveConfig},
	m_MoistureOctaveNoise {MoistureMapConfig.OctaveConfig}
{

}

float WorldGenerator::GenerateHeight(int blockX, int blockZ) const
{
	const float noise = m_HeightOctaveNoise.Sample(blockX, blockZ);
	const float expNoise = std::powf(noise * 1.2f, 1.5f);
	return expNoise;
}

float WorldGenerator::GenerateMoisture(int blockX, int blockZ) const
{
	const float x = blockX + MoistureMapConfig.Offset;
	const float z = blockZ + MoistureMapConfig.Offset;
	const float noise = m_MoistureOctaveNoise.Sample(x, z);
	return noise;
}

Biome WorldGenerator::GetBiome(float height, float moisture) const
{
	if (height < 0.3f)
	{
		return Biome::Ocean;
	}
	if (height < 0.31f)
	{
		return Biome::Beach;
	}
	if (height < 0.6f)
	{
		if (moisture < 0.4f)
		{
			return Biome::Desert;
		}
		else if (moisture < 0.6f)
		{
			return Biome::Plains;
		}
		else
		{
			return Biome::Forest;
		}
	}
	if (height < 0.98f)
	{
		return Biome::Mountains;
	}
	return Biome::SnowyMountains;
}

ChunkHeightMap WorldGenerator::GenerateHeightMap(ChunkCoords2D coords) const
{
	ChunkHeightMap heightMap;
	for (uint8_t z = 0; z < CHUNK_DIMENSION; z++)
	{
		for (uint8_t x = 0; x < CHUNK_DIMENSION; x++)
		{
			const float height = GenerateHeight(coords.X * CHUNK_DIMENSION + x, coords.Z * CHUNK_DIMENSION + z);
			heightMap[ChunkUtils::PackXZ(x, z)] = height;
		}
	}
	return heightMap;
}

ChunkMoistureMap WorldGenerator::GenerateMoistureMap(ChunkCoords2D coords) const
{
	ChunkMoistureMap moistureMap;
	for (uint8_t z = 0; z < CHUNK_DIMENSION; z++)
	{
		for (uint8_t x = 0; x < CHUNK_DIMENSION; x++)
		{
			const float moisture = GenerateMoisture(coords.X * CHUNK_DIMENSION + x, coords.Z * CHUNK_DIMENSION + z);
			moistureMap[ChunkUtils::PackXZ(x, z)] = moisture;
		}
	}
	return moistureMap;
}

TerrainFeature WorldGenerator::GenerateTerrainFeature(ChunkCoords2D chunkCoords, LocalBlockCoords2D blockCoords, Biome biome) const
{
	const float noise = Noise::PerlinNoise(
		(blockCoords.X + chunkCoords.X * CHUNK_DIMENSION) * TerrainFeaturesConfig.Frequency,
		(blockCoords.Z + chunkCoords.Z * CHUNK_DIMENSION) * TerrainFeaturesConfig.Frequency
	);
	if (biome == Biome::Forest)
	{
		if (noise > TerrainFeaturesConfig.ForestCutOff) return TerrainFeature::Tree;
	}
	else if (biome == Biome::Plains)
	{
		if (noise > TerrainFeaturesConfig.PlainsCutoff) return TerrainFeature::Tree;
	}
	return TerrainFeature::None;
}

void WorldGenerator::BuildTerrainFeature(Chunk& chunk, LocalBlockCoords originBlockCoords, TerrainFeature feature)
{
	if (feature == TerrainFeature::None) return;
	std::span<RelativeBlockPlacement> blocks = GetFeatureBlocks(feature);

	const ChunkCoords chunkCoords = chunk.GetCoords();
	const BlockCoords chunkBlockCoordsStart = 
	{
		chunkCoords.X * CHUNK_DIMENSION,
		chunkCoords.Y * CHUNK_DIMENSION, 
		chunkCoords.Z * CHUNK_DIMENSION
	};

	for (RelativeBlockPlacement block : blocks)
	{
		const BlockCoords blockCoords = originBlockCoords + block.Coords;
		const LocalBlockCoords localCoords = static_cast<LocalBlockCoords>(blockCoords);
		if (ChunkUtils::IsLocal(blockCoords))
		{
			chunk.SetBlock(block.Block, localCoords.ToIndex());
		}
		else
		{
			const ChunkCoords neighborChunkCoords = static_cast<ChunkCoords>(chunkBlockCoordsStart + blockCoords);
			if (Chunk* neighborChunk = m_World->GetChunk(neighborChunkCoords))
			{
				neighborChunk->SetBlock(block.Block, localCoords.ToIndex());
			}
			else
			{
				m_ToPlace[neighborChunkCoords].push_back(LocalBlockPlacement{ block.Block, localCoords.ToIndex() });
			}
		}
	}
}

void WorldGenerator::BuildTerrainFeatures(Chunk& chunk, const ChunkGenInfo& genInfo)
{
	const ChunkCoords chunkCoords = chunk.GetCoords();
	const ChunkCoords2D chunkCoords2D = static_cast<ChunkCoords2D>(chunkCoords);
	for (uint8_t z = 0; z < CHUNK_DIMENSION; z++)
	{
		for (uint8_t x = 0; x < CHUNK_DIMENSION; x++)
		{
			const float height = genInfo.HeightMap[ChunkUtils::PackXZ(x, z)];
			const float moisture = genInfo.MoistureMap[ChunkUtils::PackXZ(x, z)];
			const int featureStartY = BlockHeightFromFloat(genInfo.HeightMap[ChunkUtils::PackXZ(x, z)]) + 1;

			const Biome biome = GetBiome(height, moisture);

			if (ChunkUtils::BlockToChunkSpace(featureStartY) == chunkCoords.Y)
			{
				const TerrainFeature feature = GenerateTerrainFeature(chunkCoords2D, { x, z }, biome);
				const uint8_t y = ChunkUtils::BlockToLocalSpace(featureStartY);
				BuildTerrainFeature(chunk, {x, y, z}, feature);
			}
		}
	}
	auto it = m_ToPlace.find(chunkCoords);
	if (it != m_ToPlace.end())
	{
		const std::vector<LocalBlockPlacement>& allToPlace = it->second;
		for (LocalBlockPlacement toPlace : allToPlace)
		{
			chunk.SetBlock(toPlace.Block, toPlace.Index);
		}
		m_ToPlace.erase(it);
	}
}

const ChunkGenInfo& WorldGenerator::GetChunkGenInfo(ChunkCoords2D coords)
{
	const ChunkGenInfo* heightMap = m_Cache.Get(coords);
	if (heightMap) return *heightMap;

	return m_Cache.Insert(coords, ChunkGenInfo{ GenerateHeightMap(coords), GenerateMoistureMap(coords) });
}

static BlockType GetSurfaceBlock(Biome biome)
{
	switch (biome)
	{
	case Biome::Beach:
	case Biome::Desert:
	case Biome::Ocean:
		return BlockType::Sand;
	case Biome::Forest:
	case Biome::Plains:
		return BlockType::Grass;
	case Biome::Mountains:
		return BlockType::Stone;
	case Biome::SnowyMountains:
		return BlockType::Snow;
	default:
		return BlockType::Stone;
	}
}

static BlockType GetGroundBlock(Biome biome, int depth)
{
	switch (biome)
	{
	case Biome::Beach:
	case Biome::Desert:
	case Biome::Ocean:
		if (depth < 3) return BlockType::Sand; else return BlockType::Stone;
	case Biome::Forest:
	case Biome::Plains:
		if (depth < 3) return BlockType::Dirt; else return BlockType::Stone;
	case Biome::Mountains:
		return BlockType::Stone;
	case Biome::SnowyMountains:
		if (depth < 2) return BlockType::Snow; else return BlockType::Stone;
	default:
		return BlockType::Stone;
	}
}

BlockType WorldGenerator::GetBlock(int surfaceHeight, int blockHeight, Biome biome) const
{
	if (blockHeight < -20) return BlockType::Air;

	if (blockHeight > surfaceHeight)
	{
		if (biome == Biome::Ocean && blockHeight < 30) return BlockType::Water;
		else return BlockType::Air;
	}
	else if (blockHeight == surfaceHeight)
	{
		return GetSurfaceBlock(biome);
	}
	else
	{
		return GetGroundBlock(biome, surfaceHeight - blockHeight);
	}
	/*else if (blockHeight >= surfaceHeight - 2)
	{
		switch (biome)
		{
		case Biome::Beach:
		
		}
		return BlockType::Dirt;
	}
	else
	{
		return BlockType::Stone;
	}*/
}

void WorldGenerator::BuildTerrain(Chunk& chunk, const ChunkGenInfo& genInfo) const
{
	const ChunkCoords chunkCoords = chunk.GetCoords();
	for (uint8_t z = 0; z < CHUNK_DIMENSION; z++)
	{
		for (uint8_t x = 0; x < CHUNK_DIMENSION; x++)
		{
			const size_t i = ChunkUtils::PackXZ(x, z);
			const float height = genInfo.HeightMap[i];
			const float moisture = genInfo.MoistureMap[i];

			const int surfaceHeight = BlockHeightFromFloat(height);
			const Biome biome = GetBiome(height, moisture);
			for (uint8_t y = 0; y < CHUNK_DIMENSION; y++)
			{
				const int blockHeight = chunkCoords.Y * CHUNK_DIMENSION + y;
				const BlockType block = GetBlock(surfaceHeight, blockHeight, biome);
				chunk.SetBlock(block, x, y, z);
			}
		}
	}
}

std::unique_ptr<Chunk> WorldGenerator::GenerateChunk(ChunkCoords chunkCoords)
{
	const ChunkGenInfo& genInfo = GetChunkGenInfo(static_cast<ChunkCoords2D>(chunkCoords));

	auto chunk = std::make_unique<Chunk>(chunkCoords);

	BuildTerrain(*chunk, genInfo);

	BuildTerrainFeatures(*chunk, genInfo);

	return chunk;
} 