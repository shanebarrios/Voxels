#pragma once

#include "Block.h"
#include "ChunkMesh.h"
#include "../Coordinates.h"
#include <array>
#include <glm/glm.hpp>

class World;

class Chunk
{
public:
	static constexpr size_t k_Width = 16;
	static constexpr size_t k_Depth = 16;
	static constexpr size_t k_Height = 16;
	static constexpr size_t k_Size = k_Width * k_Depth * k_Height;


	Chunk();
	explicit Chunk(ChunkCoords coords);

	ChunkCoords GetCoords() const { return m_Coords; };

	const ChunkMesh& GetMesh() const { return m_Mesh; }

	BlockType GetBlock(size_t i) const;
	BlockType GetBlock(uint8_t x, uint8_t y, uint8_t z) const;

	void SetBlock(BlockType blockType, size_t i);
	void SetBlock(BlockType blockType, uint8_t x, uint8_t y, uint8_t z);

	const std::array<BlockType, k_Size>& GetBlocks() const { return m_Blocks; }

	void RebuildMesh(const World& world);
	void TriggerRebuild() { m_NeedsRebuild = true; }
	bool NeedsRebuild() const { return m_NeedsRebuild; }

private:
	std::array<BlockType, k_Size> m_Blocks{};
	ChunkCoords m_Coords{};
	ChunkMesh m_Mesh{};

	bool m_NeedsRebuild = true;
};