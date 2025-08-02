#include "ChunkMesh.h"
#include <vector>
#include <utility>
#include "Chunk.h"
#include "ChunkUtils.h"
#include "World.h"

// Move this to separate header eventually
#if defined (_MSC_VER) && !defined(__clang__)
	#define unreachable() __assume(false)
#else
	#define unreachable() __builtin_unreachable()
#endif

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

// Assumes that the chunk vertex has not already been offset
static uint8_t GetOcclusionFactor(const Chunk& chunk, const World& world, ChunkVertex vertex, LocalBlockCoords offset)
{
	const LocalBlockCoords blockLocalCoords = vertex.GetLocalCoords();
	const int dx = static_cast<int>(blockLocalCoords.X * 2) - 1;
	const int dy = static_cast<int>(blockLocalCoords.Y * 2) - 1;
	const int dz = static_cast<int>(blockLocalCoords.Z * 2) - 1;

	// Use BlockCoords instead of LocalBlockCoords because it might be outside of chunk
	BlockCoords edge1Coords;
	BlockCoords edge2Coords;
	BlockCoords cornerCoords;
	switch (vertex.GetFace())
	{
	case BlockFace::PosZ:
		edge1Coords = { offset.X + dx, offset.Y, offset.Z + 1 };
		edge2Coords = { offset.X, offset.Y + dy, offset.Z + 1 };
		cornerCoords = { offset.X + dx, offset.Y + dy, offset.Z + 1 };
		break;
	case BlockFace::NegZ:
		edge1Coords = { offset.X + dx, offset.Y, offset.Z - 1 };
		edge2Coords = { offset.X, offset.Y + dy, offset.Z - 1 };
		cornerCoords = { offset.X + dx, offset.Y + dy, offset.Z - 1 };
		break;
	case BlockFace::NegX:
		edge1Coords = { offset.X - 1, offset.Y + dy, offset.Z};
		edge2Coords = { offset.X - 1, offset.Y, offset.Z + dz };
		cornerCoords = { offset.X - 1, offset.Y + dy, offset.Z + dz };
		break;
	case BlockFace::PosX:
		edge1Coords = { offset.X + 1, offset.Y + dy, offset.Z };
		edge2Coords = { offset.X + 1, offset.Y, offset.Z + dz };
		cornerCoords = { offset.X + 1, offset.Y + dy, offset.Z + dz };
		break;
	case BlockFace::PosY:
		edge1Coords = { offset.X + dx, offset.Y + 1, offset.Z };
		edge2Coords = { offset.X, offset.Y + 1, offset.Z + dz };
		cornerCoords = { offset.X + dx, offset.Y + 1, offset.Z + dz };
		break;
	case BlockFace::NegY:
		edge1Coords = { offset.X + dx, offset.Y - 1, offset.Z };
		edge2Coords = { offset.X, offset.Y - 1, offset.Z + dz };
		cornerCoords = { offset.X + dx, offset.Y - 1, offset.Z + dz };
		break;
	default:
		unreachable();
	}
	const bool edge1 = !IsTranslucent(GetBlock(chunk, world, edge1Coords));
	const bool edge2 = !IsTranslucent(GetBlock(chunk, world, edge2Coords));
	const bool corner = !IsTranslucent(GetBlock(chunk, world, cornerCoords));
	if (edge1 && edge2) return 0;
	return 3 - (edge1 + edge2 + corner);
}

// Per frame heap allocations are slow and this is too big for the stack
static ChunkVertex s_Buffer[16 * 16 * 16 * 6 * 6];
static ChunkVertex s_TransparentBuffer[16 * 16 * 16 * 6 * 6];

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
	const BlockType* blocks = chunk.GetBlocks();
	for (size_t i = 0; i < Chunk::k_Size; i++)
	{
		HandleBlock(chunk, world, i);
	}
	m_OpaqueVBO.SetData(s_Buffer, m_BufferIndex);
	m_TransparentVBO.SetData(s_TransparentBuffer, m_TransparentBufferIndex);
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

		if ((!IsTranslucent(block) && !IsTranslucent(neighborBlock)) ||
			(IsTransparent(block) && neighborBlock != BlockType::Air)) continue;

		//if (!IsTransparent(neighborBlock) || neighborBlock == block) continue;

		AddFace(chunk, world, static_cast<BlockFace>(face), block, localCoords);
	}
}

void ChunkMesh::AddFace(const Chunk& chunk, const World& world, BlockFace face, BlockType blockType, LocalBlockCoords offset)
{
	for (size_t i = 0; i < ChunkVertex::k_VerticesPerFace; i++)
	{
		ChunkVertex vertex = k_FaceVertices[static_cast<size_t>(face)][i];
		vertex.SetAmbientOcclusion(GetOcclusionFactor(chunk, world, vertex, offset));
		vertex.Offset(offset.X, offset.Y, offset.Z);
		vertex.SetTextureIndex(GetTextureIndex(face, blockType));

		if (blockType == BlockType::Water)
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