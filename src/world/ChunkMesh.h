#pragma once

#include <array>
#include "ChunkVertex.h"
#include "../rendering/VertexArray.h"
#include "../rendering/Buffer.h"
#include "Block.h"

class Chunk;
class World;

class ChunkMesh
{
public:
	ChunkMesh();

	size_t NumVertices() const;
	void Rebuild(const Chunk& chunk, const World& world);
	void Bind() const;
private:
	// Per frame heap allocations are slow and this is too big for the stack
	static std::array<ChunkVertex, 16 * 16 * 16 * 6 * 6> s_Buffer;

	size_t m_BufferIndex = 0;
	VertexBuffer m_VBO{};
	VertexArray m_VAO{};
	// No index buffer because vertices take up only 4 bytes

	void AddFace(const Chunk& chunk, BlockFace face, BlockType blockType, uint8_t xOffset, uint8_t yOffset, uint8_t zOffset);
};

