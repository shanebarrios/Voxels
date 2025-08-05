#pragma once

#include <cstdint>
#include <numbers>
#include <array>
#include <optional>
#include "Utils/Coordinates.h"

namespace ChunkUtils
{
	inline constexpr std::array<BlockCoords, static_cast<size_t>(BlockFace::Count)> k_FaceNormals
	{{
			// Front
			{ 0, 0, 1 },
			// Back
			{ 0, 0, -1 },
			// Left
			{ -1, 0, 0 },
			// Right
			{ 1, 0, 0 },
			// Top
			{ 0, 1, 0 },
			// Bottom
			{ 0, -1, 0 }
	}};

	inline size_t NeighboringChunks(ChunkCoords chunkCoords, uint8_t blockX, uint8_t blockY, uint8_t blockZ, std::array<ChunkCoords, 3>& neighbors)
	{
		size_t i = 0;
		if (blockX == 0)
		{
			neighbors[i++] = { chunkCoords.X - 1, chunkCoords.Y, chunkCoords.Z };
		}
		if (blockX == CHUNK_DIMENSION-1)
		{
			neighbors[i++] = { chunkCoords.X + 1, chunkCoords.Y, chunkCoords.Z };
		}
		if (blockY == 0)
		{
			neighbors[i++] = { chunkCoords.X, chunkCoords.Y - 1, chunkCoords.Z };
		}
		if (blockY == CHUNK_DIMENSION-1)
		{
			neighbors[i++] = { chunkCoords.X, chunkCoords.Y + 1, chunkCoords.Z };
		}
		if (blockZ == 0)
		{
			neighbors[i++] = { chunkCoords.X, chunkCoords.Y, chunkCoords.Z - 1 };
		}
		if (blockZ == CHUNK_DIMENSION-1)
		{
			neighbors[i++] = { chunkCoords.X, chunkCoords.Y, chunkCoords.Z + 1 };
		}
		return i;
	}

	inline constexpr bool BlockInChunkBounds(BlockCoords blockCoords, ChunkCoords chunkCoords)
	{
		return static_cast<ChunkCoords>(blockCoords) == chunkCoords;
	}

	inline constexpr bool IsLocal(BlockCoords blockCoords)
	{
		return 
			blockCoords.X >= 0 && blockCoords.X < CHUNK_DIMENSION && 
			blockCoords.Y >= 0 && blockCoords.Y < CHUNK_DIMENSION && 
			blockCoords.Z >= 0 && blockCoords.Z < CHUNK_DIMENSION;
	}

	inline int BlockToChunkSpace(int blockIndex)
	{
		return blockIndex >= 0 ? 
			blockIndex / CHUNK_DIMENSION : 
			(blockIndex - CHUNK_DIMENSION + 1) / CHUNK_DIMENSION;
	}

	inline uint8_t BlockToLocalSpace(int blockIndex)
	{
		return static_cast<uint8_t>(blockIndex & CHUNK_COORD_MASK);
	}

	inline constexpr uint8_t ExtractX(size_t index)
	{
		return index & CHUNK_COORD_MASK;
	}

	inline constexpr uint8_t ExtractZ(size_t index)
	{
		return (index >> CHUNK_COORD_BIT_COUNT) & CHUNK_COORD_MASK;
	}

	inline constexpr uint8_t ExtractY(size_t index)
	{
		return (index >> CHUNK_COORD_BIT_COUNT * 2) & CHUNK_COORD_MASK;
	}

	inline constexpr LocalBlockCoords ExtractLocalBlockCoords(size_t index)
	{
		return
		{
			ExtractX(index),
			ExtractY(index),
			ExtractZ(index)
		};
	}

	inline constexpr size_t PackXYZ(uint8_t x, uint8_t y, uint8_t z)
	{
		return 
			static_cast<size_t>(x) | 
			(static_cast<size_t>(z) << CHUNK_COORD_BIT_COUNT) | 
			(static_cast<size_t>(y) << CHUNK_COORD_BIT_COUNT * 2);
	}

	inline constexpr size_t PackXZ(uint8_t x, uint8_t z)
	{
		return 
			static_cast<size_t>(x) | 
			static_cast<size_t>(z) << CHUNK_COORD_BIT_COUNT;
	}
}