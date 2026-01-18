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

Chunk* ChunkAllocator::AllocChunk()
{
    BlockType* const blockData = m_BlockDataAllocator.Alloc<BlockType>();
    Chunk* const chunk = m_ChunkPoolAllocator.New<Chunk>();
    chunk->SetBlocks(blockData);
    return chunk;
}

void ChunkAllocator::FreeChunk(Chunk* chunk)
{
    m_BlockDataAllocator.Dealloc(chunk->GetBlocks());
    m_ChunkPoolAllocator.Dealloc(chunk);
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
