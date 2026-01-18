#include "Chunk.h"
#include "ChunkUtils.h"
#include <cassert>

Chunk::Chunk() : Chunk{ChunkCoords{}, nullptr} {}

Chunk::Chunk(ChunkCoords coords, BlockType* blocks)
    : m_Coords{coords}, m_Blocks{blocks}
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
    if (blockType != BlockType::Air)
        m_PotentiallyHasBlocks = true;
    m_Blocks[i] = blockType;
}

void Chunk::SetBlock(BlockType blockType, uint8_t x, uint8_t y, uint8_t z)
{
    m_NeedsRebuild = true;
    if (blockType != BlockType::Air)
        m_PotentiallyHasBlocks = true;
    m_Blocks[ChunkUtils::PackXYZ(x, y, z)] = blockType;
}

void Chunk::RebuildMesh(const World& world)
{
    m_Mesh.Rebuild(*this, world);
    m_NeedsRebuild = false;
}
