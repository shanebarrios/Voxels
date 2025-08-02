#pragma once

#include <cstdint>
#include "Block.h"
#include "Utils/Coordinates.h"

class BufferLayout;

// Bits 0-4   : X offset
// Bits 5-9   : Y offset
// Bits 10-14 : Z offset
// Bits 15-22 : Texture index
// Bit 23     : Texture U offset
// Bit 24     : Texture V offset
// Bits 25-27 : Block face
// Bits 28-29 : Ambient occlusion

class ChunkVertex
{
public:
	static const BufferLayout& GetBufferLayout();
	static constexpr size_t k_VerticesPerFace = 6;

	constexpr ChunkVertex() = default;
	constexpr ChunkVertex(uint8_t offsetX, uint8_t offsetY, uint8_t offsetZ, uint8_t u, uint8_t v, BlockFace blockFace) :
		m_Encoding(offsetX | (offsetY << 5u) | (offsetZ << 10u) | (u << 23u) | (v << 24u) | (static_cast<uint8_t>(blockFace) << 25u))
	{
	}

	LocalBlockCoords GetLocalCoords() const
	{
		return
		{
			static_cast<uint8_t>(m_Encoding & 0x1Fu),
			static_cast<uint8_t>((m_Encoding >> 5u) & 0x1Fu),
			static_cast<uint8_t>((m_Encoding >> 10u) & 0x1Fu)
		};
	}

	BlockFace GetFace() const
	{
		return static_cast<BlockFace>((m_Encoding >> 25u) & 0x7u);
	}

	// Offset within the chunk, x, y, z should be in range [0, 15]
	void Offset(uint8_t x, uint8_t y, uint8_t z);

	// Assumes texture index has not been set yet! Should be in range [0, 255]
	void SetTextureIndex(uint8_t i);

	// Should be in range [0, 3]
	void SetAmbientOcclusion(uint8_t amount);

	uint32_t Get() const { return m_Encoding; }

private:
	uint32_t m_Encoding = 0;
};