#include "ChunkVertex.h"
#include "Chunk.h"
#include <cassert>
#include "Rendering/VertexArray.h"
#include <array>

const BufferLayout& ChunkVertex::GetBufferLayout()
{
    static BufferLayout layout{{LayoutElementType::UInt, 1},
                               {LayoutElementType::UInt, 1}};
    return layout;
}

void ChunkVertex::Offset(uint8_t x, uint8_t y, uint8_t z)
{
    assert(x <= CHUNK_DIMENSION && y <= CHUNK_DIMENSION &&
           z <= CHUNK_DIMENSION && "Position out of bounds!");
    m_Encoding += static_cast<size_t>(x) | (static_cast<size_t>(y) << 6u) |
                  (static_cast<size_t>(z) << 12u);
}

void ChunkVertex::SetTextureIndex(uint8_t i)
{
    m_Encoding |= (static_cast<size_t>(i) << 18u);
}

void ChunkVertex::SetAmbientOcclusion(uint8_t amount)
{
    m_Encoding |= (static_cast<size_t>(amount) << 32u);
}