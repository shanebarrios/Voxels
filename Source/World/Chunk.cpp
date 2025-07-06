#include "Chunk.h"
#include "ChunkUtils.h"
#include "Math/Noise.h"
#include <random>
#include <cassert>
#include "Logger.h"

Chunk::Chunk() :
	Chunk{ ChunkCoords {} }
{
	
}

Chunk::Chunk(ChunkCoords coords) :
	m_Coords{coords}
{	
}

BlockType Chunk::GetBlock(size_t i) const
{
	return m_Blocks[i];
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