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

std::array<ChunkVertex, 16 * 16 * 16 * 6 * 6> ChunkMesh::s_Buffer{};

ChunkMesh::ChunkMesh()
{
	const BufferLayout& layout = ChunkVertex::GetBufferLayout();
	m_VAO.SetVertexBuffer(m_VBO, layout);
}

size_t ChunkMesh::NumVertices() const
{
	return m_BufferIndex;
}

void ChunkMesh::Rebuild(const Chunk& chunk, const World& world)
{
	m_BufferIndex = 0;
	const std::array<BlockType, Chunk::k_Size>& blocks = chunk.GetBlocks();
	for (size_t i = 0; i < blocks.size(); i++)
	{
		if (blocks[i] == BlockType::Air)
		{
			continue;
		}
		const uint8_t x = ChunkUtils::ExtractX(i);
		const uint8_t y = ChunkUtils::ExtractY(i);
		const uint8_t z = ChunkUtils::ExtractZ(i);
		for (size_t face = 0; face < static_cast<size_t>(BlockFace::Count); face++)
		{
			const std::array<int, 3>& normal = ChunkUtils::k_FaceNormals[face];
			const int neighborX = x + normal[0];
			const int neighborY = y + normal[1];
			const int neighborZ = z + normal[2];

			if (InChunkBounds(neighborX, neighborY, neighborZ))
			{
				if (blocks[ChunkUtils::PackXYZ(static_cast<uint8_t>(neighborX), static_cast<uint8_t>(neighborY), static_cast<uint8_t>(neighborZ))] != BlockType::Air)
				{
					continue;
				}
			}
			else
			{
				const ChunkCoords chunkCoords = chunk.GetCoords();
				const BlockCoords blockCoords =
					BlockCoords{ chunkCoords.X * 16, chunkCoords.Y * 16, chunkCoords.Z * 16 } +
					BlockCoords{ neighborX, neighborY, neighborZ };
				if (world.GetBlock(blockCoords) != BlockType::Air)
				{
					continue;
				}
			}

			AddFace(chunk, static_cast<BlockFace>(face), blocks[i], x, y, z);
		}
	}
	m_VBO.SetData(s_Buffer.data(), m_BufferIndex);
}

void ChunkMesh::AddFace(const Chunk& chunk, BlockFace face, BlockType blockType, uint8_t xOffset, uint8_t yOffset, uint8_t zOffset)
{
	for (size_t i = 0; i < ChunkVertex::k_VerticesPerFace; i++)
	{
		ChunkVertex vertex = k_FaceVertices[static_cast<size_t>(face)][i];
		vertex.Offset(xOffset, yOffset, zOffset);
		vertex.SetTextureIndex(GetTextureIndex(face, blockType));
		s_Buffer[m_BufferIndex++] = vertex;
	}
}

void ChunkMesh::Bind() const
{
	m_VAO.Bind();
}