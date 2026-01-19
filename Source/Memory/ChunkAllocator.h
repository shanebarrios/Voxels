#pragma once

#include "PoolAllocator.h"
#include "World/Block.h"

class Chunk;

class ChunkAllocator
{
  public:
    ChunkAllocator() = default;
    explicit ChunkAllocator(size_t maxChunks);

    void Init(size_t maxChunks);

    void* AllocChunk();
    BlockType* AllocBlockData();

    void FreeChunk(void* chunk);
    void FreeBlockData(BlockType* blocks);

    void Reset();
    void Free();

  private:
    PoolAllocator m_ChunkPoolAllocator{};
    PoolAllocator m_BlockDataAllocator{};
};

inline ChunkAllocator g_ChunkAllocator;
