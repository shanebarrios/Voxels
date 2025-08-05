#pragma once

#include "Utils/Common.h"
#include "Block.h"
#include "Utils/Common.h"
#include "ChunkMesh.h"
#include "Utils/Coordinates.h"

class World;

class Chunk
{
public:
	Chunk();
	explicit Chunk(ChunkCoords coords);

	ChunkCoords GetCoords() const { return m_Coords; };

	const ChunkMesh& GetMesh() const { return m_Mesh; }

	BlockType GetBlock(size_t i) const;
	BlockType GetBlock(uint8_t x, uint8_t y, uint8_t z) const;

	void SetBlock(BlockType blockType, size_t i);
	void SetBlock(BlockType blockType, uint8_t x, uint8_t y, uint8_t z);

	BlockType* GetBlocks() { return m_Blocks; }
	const BlockType* GetBlocks() const { return m_Blocks; }

	void RebuildMesh(const World& world);
	void TriggerRebuild() { m_NeedsRebuild = true; }
	bool NeedsRebuild() const { return m_NeedsRebuild && m_PotentiallyHasBlocks; }

private:
	BlockType m_Blocks[CHUNK_VOLUME]{};
	ChunkCoords m_Coords{};
	ChunkMesh m_Mesh{};

	bool m_NeedsRebuild = false;
	bool m_PotentiallyHasBlocks = false;
};