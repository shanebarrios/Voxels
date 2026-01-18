#pragma once

#include "Block.h"
class Chunk;

class ChunkAllocator
{
public:
    explicit ChunkAllocator(size_t maxChunks);
    ~ChunkAllocator();

    ChunkAllocator(const ChunkAllocator&) = delete;
    ChunkAllocator(ChunkAllocator&&);

    ChunkAllocator& operator=(const ChunkAllocator&) = delete;
    ChunkAllocator& operator=(ChunkAllocator&&);

    Chunk* AllocChunk();
    void FreeChunk(Chunk* chunk);

    void Reset();
    
private:
     
};
