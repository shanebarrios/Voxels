#include "Chunk.h"
#include "ChunkUtils.h"
#include <random>

Chunk::Chunk() :
	Chunk{ ChunkCoords {} }
{
	
}

Chunk::Chunk(ChunkCoords coords) :
	m_Coords{coords}
{
	if (coords.Y > 0 || coords.Y < -4)
	{
		return;
	}
	// TODO: replace this with actual terrain generation
	for (uint8_t y = 0; y < 16; y++)
	{
		for (uint8_t z = 0; z < 16; z++)
		{
			for (uint8_t x = 0; x < 16; x++)
			{
				m_Blocks[ChunkUtils::PackXYZ(x, y, z)] = BlockType::Grass;
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