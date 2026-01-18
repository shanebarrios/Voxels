#pragma once

#include "PoolAllocator.h"

class Chunk;

class ChunkAllocator
{
  public:
    ChunkAllocator() = default;
    explicit ChunkAllocator(size_t maxChunks);

    void Init(size_t maxChunks);

    Chunk* AllocChunk();
    void FreeChunk(Chunk* chunk);

    void Reset();
    void Free();

  private:
    PoolAllocator m_ChunkPoolAllocator{};
    PoolAllocator m_BlockDataAllocator{};
};
