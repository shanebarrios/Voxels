#include "ChunkVertex.h"
#include <cassert>
#include "../Rendering/VertexArray.h"
#include <array>

const BufferLayout& ChunkVertex::GetBufferLayout()
{
	static BufferLayout layout{ {LayoutElementType::UInt, 1} };
	return layout;
} 

void ChunkVertex::Offset(uint8_t x, uint8_t y, uint8_t z)
{
	assert(x <= 16 && y <= 16 && z <= 16 && "Position out of bounds!");
	m_Encoding += x | (y << 5u) | (z << 10u);
}

void ChunkVertex::SetTextureIndex(uint8_t i)
{
	m_Encoding |= (i << 15u);
}

void ChunkVertex::SetAmbientOcclusion(uint8_t amount)
{
	m_Encoding |= (amount << 28u);
}