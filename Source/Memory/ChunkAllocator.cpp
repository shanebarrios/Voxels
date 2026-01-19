#include "ChunkAllocator.h"
#include "../Core/Common.h"
#include "World/Block.h"
#include "World/Chunk.h"

ChunkAllocator::ChunkAllocator(size_t maxChunks)
    : m_ChunkPoolAllocator{sizeof(Chunk), maxChunks},
      m_BlockDataAllocator{sizeof(BlockType) * CHUNK_VOLUME, maxChunks}

{
}

void ChunkAllocator::Init(size_t maxChunks)
{
    m_ChunkPoolAllocator.AllocPool(sizeof(Chunk), maxChunks);
    m_BlockDataAllocator.AllocPool(sizeof(BlockType) * CHUNK_VOLUME, maxChunks);
}

void* ChunkAllocator::AllocChunk()
{
    return m_ChunkPoolAllocator.Alloc<Chunk>();
}

BlockType* ChunkAllocator::AllocBlockData()
{
    return m_BlockDataAllocator.Alloc<BlockType>();
}

void ChunkAllocator::FreeChunk(void* chunk)
{
    m_ChunkPoolAllocator.DeallocRaw(chunk);
}

void ChunkAllocator::FreeBlockData(BlockType* blocks)
{
    m_BlockDataAllocator.Dealloc(blocks);
}

void ChunkAllocator::Reset()
{
    m_BlockDataAllocator.ResetPool();
    m_ChunkPoolAllocator.ResetPool();
}

void ChunkAllocator::Free()
{
    m_BlockDataAllocator.FreePool();
    m_ChunkPoolAllocator.FreePool();
}
