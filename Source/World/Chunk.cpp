#include "Chunk.h"
#include "ChunkUtils.h"
#include "Memory/ChunkAllocator.h"
#include <cassert>

Chunk::Chunk() : Chunk{ChunkCoords{}} {}

Chunk::Chunk(ChunkCoords coords) : m_Coords{coords}
{
    m_Blocks = g_ChunkAllocator.AllocBlockData();
}

Chunk::~Chunk()
{
    if (m_Blocks)
        g_ChunkAllocator.FreeBlockData(m_Blocks);
    m_Blocks = nullptr;
}

Chunk::Chunk(Chunk&& other)
    : m_Blocks{other.m_Blocks}, m_Coords{other.m_Coords},
      m_Mesh{std::move(other.m_Mesh)}, m_NeedsRebuild{other.m_NeedsRebuild},
      m_PotentiallyHasBlocks{other.m_PotentiallyHasBlocks}
{
    other.m_Blocks = nullptr;
}

Chunk& Chunk::operator=(Chunk&& other)
{
    if (&other == this)
        return *this;

    if (m_Blocks)
        g_ChunkAllocator.FreeBlockData(m_Blocks);

    m_Blocks = other.m_Blocks;
    m_Coords = other.m_Coords;
    m_Mesh = std::move(other.m_Mesh);
    m_NeedsRebuild = other.m_NeedsRebuild;
    m_PotentiallyHasBlocks = other.m_PotentiallyHasBlocks;

    other.m_Blocks = nullptr;

    return *this;
}

void* Chunk::operator new(size_t size)
{
    if (size != sizeof(Chunk))
        throw std::bad_alloc();

    return g_ChunkAllocator.AllocChunk();
}

void Chunk::operator delete(void* ptr)
{
    if (ptr)
        g_ChunkAllocator.FreeChunk(ptr);
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
