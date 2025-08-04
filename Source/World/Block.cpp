#include "Block.h"
#include <array>
#include <glm/glm.hpp>

static constexpr std::array<std::array<uint8_t, 6>, 256> k_IndexMappings
{{
	{0, 0, 0, 0, 0, 0}, // Air (Filler)
	{1, 1, 1, 1, 1, 1}, // Grass
	{0, 0, 0, 0, 0, 0}, // Dirt
	{3, 3, 3, 3, 3, 3}, // Stone
	{4, 4, 4, 4, 5, 5}, // Log
	{6, 6, 6, 6, 6, 6}, // Leaves
	{7, 7, 7, 7, 7, 7}, // Sand
	{8, 8, 8, 8, 8, 8}, // Water
	{9, 9, 9, 9, 9, 9}, // Snow
	// Rest filled with 0s
}};

static const char* s_BlockTypeNames[] = {
	"Air",
	"Grass",
	"Dirt",
	"Stone",
	"Log",
	"Leaves",
	"Sand",
	"Water",
	"Snow"
};

uint32_t GetTextureIndex(BlockFace face, BlockType blockType)
{
	assert(blockType != BlockType::Air && "Invalid block type");
	return k_IndexMappings[static_cast<uint8_t>(blockType)][static_cast<uint8_t>(face)];
}

bool IsTransparent(BlockType blockType)
{
	switch (blockType)
	{
	case BlockType::Air:
	case BlockType::Water:
		return true;
	default:
		return false;
	}
}

bool IsTranslucent(BlockType blockType)
{
	switch (blockType)
	{
	case BlockType::Air:
	case BlockType::Water:
	case BlockType::Leaves:
		return true;
	default:
		return false;
	}
}

bool IsInteractable(BlockType blockType)
{
	return blockType != BlockType::Water && blockType != BlockType::Air;
}

const char* BlockTypeToStr(BlockType blockType)
{
	return s_BlockTypeNames[static_cast<uint8_t>(blockType)];
}

const char** GetAllBlockTypeNames()
{
	return s_BlockTypeNames;
}