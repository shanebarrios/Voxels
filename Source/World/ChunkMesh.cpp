#include "ChunkMesh.h"
#include <vector>
#include "Chunk.h"
#include "ChunkUtils.h"
#include "World.h"

static constexpr std::array<std::array<ChunkVertex, ChunkVertex::k_VerticesPerFace>, static_cast<size_t>(BlockFace::Count)> k_FaceVertices
{{
	// Front
	{{
		{0, 0, 1, 0, 0, BlockFace::PosZ},
		{1, 1, 1, 1, 1, BlockFace::PosZ},
		{0, 1, 1, 0, 1, BlockFace::PosZ},
		{0, 0, 1, 0, 0, BlockFace::PosZ},
		{1, 0, 1, 1, 0, BlockFace::PosZ},
		{1, 1, 1, 1, 1, BlockFace::PosZ}
	}},
	// Back
	{{
		{1, 0, 0, 0, 0, BlockFace::NegZ},
		{0, 1, 0, 1, 1, BlockFace::NegZ},
		{1, 1, 0, 0, 1, BlockFace::NegZ},
		{1, 0, 0, 0, 0, BlockFace::NegZ},
		{0, 0, 0, 1, 0, BlockFace::NegZ},
		{0, 1, 0, 1, 1, BlockFace::NegZ}
	}},
	// Left
	{{
		{0, 0, 0, 0, 0, BlockFace::NegX},
		{0, 1, 1, 1, 1, BlockFace::NegX},
		{0, 1, 0, 0, 1, BlockFace::NegX},
		{0, 0, 0, 0, 0, BlockFace::NegX},
		{0, 0, 1, 1, 0, BlockFace::NegX},
		{0, 1, 1, 1, 1, BlockFace::NegX}
	}},
	// Right
	{{
		{1, 0, 1, 0, 0, BlockFace::PosX},
		{1, 1, 0, 1, 1, BlockFace::PosX},
		{1, 1, 1, 0, 1, BlockFace::PosX},
		{1, 0, 1, 0, 0, BlockFace::PosX},
		{1, 0, 0, 1, 0, BlockFace::PosX},
		{1, 1, 0, 1, 1, BlockFace::PosX}
	}},
	// Top
	{{
		{0, 1, 1, 0, 0, BlockFace::PosY},
		{1, 1, 0, 1, 1, BlockFace::PosY},
		{0, 1, 0, 0, 1, BlockFace::PosY},
		{0, 1, 1, 0, 0, BlockFace::PosY},
		{1, 1, 1, 1, 0, BlockFace::PosY},
		{1, 1, 0, 1, 1, BlockFace::PosY}
	}},
	// Bottom
	{{
		{0, 0, 0, 0, 0, BlockFace::NegY},
		{1, 0, 1, 1, 1, BlockFace::NegY},
		{0, 0, 1, 0, 1, BlockFace::NegY},
		{0, 0, 0, 0, 0, BlockFace::NegY},
		{1, 0, 0, 1, 0, BlockFace::NegY},
		{1, 0, 1, 1, 1, BlockFace::NegY}
	}}
}};

static constexpr bool InChunkBounds(int x, int y, int z)
{
	return x >= 0 && x < Chunk::k_Width && y >= 0 && y < Chunk::k_Height && z >= 0 && z < Chunk::k_Depth;
}

static BlockType GetBlock(const Chunk& chunk, const World& world, BlockCoords offset)
{
	if (ChunkUtils::IsLocal(offset))
	{
		return chunk.GetBlock(offset.X, offset.Y, offset.Z);
	}
	else
	{
		const ChunkCoords chunkCoords = chunk.GetCoords();
		const BlockCoords blockCoords =
			BlockCoords{ chunkCoords.X * 16, chunkCoords.Y * 16, chunkCoords.Z * 16 } + offset;
		return world.GetBlock(blockCoords);
	}
}

std::array<ChunkVertex, 16 * 16 * 16 * 6 * 6> ChunkMesh::s_Buffer{};
std::array<ChunkVertex, 16 * 16 * 16 * 6 * 6> ChunkMesh::s_TransparentBuffer{};

ChunkMesh::ChunkMesh()
{
	const BufferLayout& layout = ChunkVertex::GetBufferLayout();
	m_OpaqueVAO.SetVertexBuffer(m_OpaqueVBO, layout);
	m_TransparentVAO.SetVertexBuffer(m_TransparentVBO, layout);
}

void ChunkMesh::Rebuild(const Chunk& chunk, const World& world)
{
	m_BufferIndex = 0;
	m_TransparentBufferIndex = 0;
	const std::array<BlockType, Chunk::k_Size>& blocks = chunk.GetBlocks();
	for (size_t i = 0; i < blocks.size(); i++)
	{
		HandleBlock(chunk, world, i);
	}
	m_OpaqueVBO.SetData(s_Buffer.data(), m_BufferIndex);
	m_TransparentVBO.SetData(s_TransparentBuffer.data(), m_TransparentBufferIndex);
}

void ChunkMesh::HandleBlock(const Chunk& chunk, const World& world, size_t i)
{
	const BlockType block = chunk.GetBlock(i);
	if (block == BlockType::Air)
	{
		return;
	}
	const bool transparent = IsTransparent(block);
	const LocalBlockCoords localCoords = ChunkUtils::ExtractLocalBlockCoords(i);

	for (size_t face = 0; face < static_cast<size_t>(BlockFace::Count); face++)
	{
		const BlockCoords neighborCoords = ChunkUtils::k_FaceNormals[face] + localCoords;

		const BlockType neighborBlock = GetBlock(chunk, world, neighborCoords);

		if (!IsTransparent(neighborBlock) || neighborBlock == block) continue;

		AddFace(chunk, world, static_cast<BlockFace>(face), block, localCoords);
	}
}

void ChunkMesh::AddFace(const Chunk& chunk, const World& world, BlockFace face, BlockType blockType, LocalBlockCoords offset)
{
	for (size_t i = 0; i < ChunkVertex::k_VerticesPerFace; i++)
	{
		ChunkVertex vertex = k_FaceVertices[static_cast<size_t>(face)][i];
		vertex.Offset(offset.X, offset.Y, offset.Z);
		vertex.SetTextureIndex(GetTextureIndex(face, blockType));
		if (blockType == BlockType::Water && 
			face == BlockFace::PosY && 
			GetBlock(chunk, world, offset + BlockCoords{0, 1, 0}) != BlockType::Water)
		{
			//vertex.SetFlagTopOfWater();
		}
		if (IsTransparent(blockType))
		{
			s_TransparentBuffer[m_TransparentBufferIndex++] = vertex;
		}
		else
		{
			s_Buffer[m_BufferIndex++] = vertex;
		}
	}
}

void ChunkMesh::BindOpaque() const
{
	m_OpaqueVAO.Bind();
}

void ChunkMesh::BindTransparent() const
{
	m_TransparentVAO.Bind();
}