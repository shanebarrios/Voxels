#pragma once

#include <cstdint>
#include "Block.h"

class BufferLayout;

class ChunkVertex
{
public:
	static const BufferLayout& GetBufferLayout();
	static constexpr size_t k_VerticesPerFace = 6;

	constexpr ChunkVertex() = default;
	constexpr ChunkVertex(uint8_t offsetX, uint8_t offsetY, uint8_t offsetZ, uint8_t u, uint8_t v, BlockFace blockFace) :
		m_Encoding(offsetX | (offsetY << 5u) | (offsetZ << 10u) | (u << 23u) | (v << 24u) | (static_cast<uint8_t>(blockFace) << 25u))
	{}

	void Offset(uint8_t x, uint8_t y, uint8_t z);
	// Assumes texture index has not been set yet!
	void SetTextureIndex(uint8_t i);

	uint32_t Get() const { return m_Encoding; }

private:
	uint32_t m_Encoding = 0;
};