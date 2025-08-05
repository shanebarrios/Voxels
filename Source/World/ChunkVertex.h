#pragma once

#include <cstdint>
#include "Block.h"
#include "Utils/Coordinates.h"

class BufferLayout;

// Bits 0-5   : X offset
// Bits 6-11  : Y offset
// Bits 12-17 : Z offset
// Bits 18-25 : Texture index
// Bit  26    : Texture U offset
// Bit  27    : Texture V offset
// Bits 28-30 : Block face
// Bit 31     : Unused
// Bits 32-33 : Ambient occlusion
// Bits 34-63 : Unused

class ChunkVertex
{
public:
	static const BufferLayout& GetBufferLayout();
	static constexpr size_t VERTICES_PER_FACE = 6;

	constexpr ChunkVertex() = default;
	constexpr ChunkVertex(uint8_t offsetX, uint8_t offsetY, uint8_t offsetZ, uint8_t u, uint8_t v, BlockFace blockFace) :
		m_Encoding(offsetX | (offsetY << 6u) | (offsetZ << 12u) | (u << 26u) | (v << 27u) | (static_cast<uint8_t>(blockFace) << 28u))
	{
	}

	LocalBlockCoords GetLocalCoords() const
	{
		return
		{
			static_cast<uint8_t>(m_Encoding & 0x3Fu),
			static_cast<uint8_t>((m_Encoding >> 6u) & 0x3Fu),
			static_cast<uint8_t>((m_Encoding >> 12u) & 0x3Fu)
		};
	}

	BlockFace GetFace() const
	{
		return static_cast<BlockFace>((m_Encoding >> 28u) & 0x7u);
	}

	// Offset within the chunk, x, y, z should be in range [0, 15]
	void Offset(uint8_t x, uint8_t y, uint8_t z);

	// Assumes texture index has not been set yet! Should be in range [0, 255]
	void SetTextureIndex(uint8_t i);

	// Should be in range [0, 3]
	void SetAmbientOcclusion(uint8_t amount);

	uint64_t Get() const { return m_Encoding; }

private:
	uint64_t m_Encoding = 0;
};