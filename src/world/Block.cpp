#include "Block.h"
#include <array>
#include <glm/glm.hpp>

static constexpr std::array<std::array<uint8_t, 6>, 256> k_IndexMappings
{{
	{0, 0, 0, 0, 0, 0}, // Air (Filler)
	{2, 2, 2, 2, 1, 0}, // Grass
	{0, 0, 0, 0, 0, 0}, // Dirt
	{3, 3, 3, 3, 3, 3}  // Stone
	// Rest filled with 0s
}};

uint32_t GetTextureIndex(BlockFace face, BlockType blockType)
{
	assert(blockType != BlockType::Air && "Invalid block type");
	return k_IndexMappings[static_cast<uint8_t>(blockType)][static_cast<uint8_t>(face)];
}