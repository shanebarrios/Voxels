#pragma once

#include <cstdint>

enum class BlockFace : uint8_t
{
	PosZ = 0,
	NegZ,
	NegX,
	PosX,
	PosY,
	NegY,
	Count
};

enum class BlockType : uint8_t
{
	Air = 0,
	Grass,
	Dirt,
	Stone,
	NumBlockTypes
};

uint32_t GetTextureIndex(BlockFace face, BlockType blockType);