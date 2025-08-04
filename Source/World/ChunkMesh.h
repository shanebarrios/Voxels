#pragma once

#include <array>
#include "ChunkVertex.h"
#include "Rendering/VertexArray.h"
#include "Rendering/Buffer.h"
#include "Block.h"
#include "Utils/Coordinates.h"

class Chunk;
class World;

class ChunkMesh
{
public:
	ChunkMesh();

	void Rebuild(const Chunk& chunk, const World& world);

	size_t NumOpaqueVertices() const { return m_BufferIndex; }
	size_t NumTransparentVertices() const { return m_TransparentBufferIndex; }

	void BindOpaque() const;
	void BindTransparent() const;
	
private: 
	void HandleBlock(const Chunk& chunk, const World& world, size_t i);

	void AddFace(const Chunk& chunk, const World& world, BlockFace face, BlockType blockType, LocalBlockCoords offset);
	
private:
	size_t m_BufferIndex = 0;
	size_t m_TransparentBufferIndex = 0;
	VertexBuffer m_OpaqueVBO{};
	VertexBuffer m_TransparentVBO{};
	VertexArray m_OpaqueVAO{};
	VertexArray m_TransparentVAO{};
	// No index buffer because vertices take up only 4 bytes
};

