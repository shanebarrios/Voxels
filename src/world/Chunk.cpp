#include "Chunk.h"
#include "ChunkUtils.h"
#include "../math/Noise.h"
#include <random>
#include <cassert>
#include "../Logger.h"

Chunk::Chunk() :
	Chunk{ ChunkCoords {} }
{
	
}

Chunk::Chunk(ChunkCoords coords) :
	m_Coords{coords}
{	
	if (coords.Y < -5) return;

	// Order is important for cache reasons 
	for (uint8_t x = 0; x < 16; x++)
	{
		for (uint8_t z = 0; z < 16; z++)
		{
			const float noise = Noise::PerlinNoise((coords.X * 16 + x) * 0.4, (coords.Z * 16 + z) * 0.4);
			int height = noise * 3 + 1;
			for (uint8_t y = 0; coords.Y * 16 + y <= height - 3 && y < 16; y++)
			{
				m_Blocks[ChunkUtils::PackXYZ(x, y, z)] = BlockType::Stone;
			}
			if (height - 2 - coords.Y * 16 >= 0 && height - 2 - coords.Y * 16 < 16)
			{
				m_Blocks[ChunkUtils::PackXYZ(x, height - 2 - coords.Y * 16, z)] = BlockType::Dirt;
			}
			if (height - 1 - coords.Y * 16 >= 0 && height - 1 - coords.Y * 16 < 16)
			{
				m_Blocks[ChunkUtils::PackXYZ(x, height - 1 - coords.Y * 16, z)] = BlockType::Dirt;
			}
			if (height - coords.Y * 16 >= 0 && height - coords.Y * 16 < 16)
			{
				m_Blocks[ChunkUtils::PackXYZ(x, height - coords.Y * 16, z)] = BlockType::Grass;
			}
		}
	}
}

BlockType Chunk::GetBlock(uint8_t x, uint8_t y, uint8_t z) const
{
	return m_Blocks[ChunkUtils::PackXYZ(x, y, z)];
}

void Chunk::SetBlock(BlockType blockType, size_t i)
{
	m_NeedsRebuild = true;
	m_Blocks[i] = blockType;
}

void Chunk::SetBlock(BlockType blockType, uint8_t x, uint8_t y, uint8_t z)
{
	m_NeedsRebuild = true;
	m_Blocks[ChunkUtils::PackXYZ(x, y, z)] = blockType;
}

void Chunk::RebuildMesh(const World& world)
{
	m_Mesh.Rebuild(*this, world);
	m_NeedsRebuild = false;
}

void Chunk::BindMesh() const
{
	m_Mesh.Bind();
}