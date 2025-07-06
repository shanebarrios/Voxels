#pragma once
#include <cmath>
#include <functional>
#include <cstdint>
#include <glm/glm.hpp>

struct ChunkCoords2D
{
	int X;
	int Z;

	bool operator==(const ChunkCoords2D&) const = default;
};

struct ChunkCoords
{
	int X = 0;
	int Y = 0;
	int Z = 0;

	int NormSq() const { return X * X + Y * Y + Z * Z; }

	bool operator==(const ChunkCoords&) const = default;

	explicit operator ChunkCoords2D() const
	{
		return { X, Z };
	}

	constexpr ChunkCoords operator+(const ChunkCoords& other) const
	{
		return
		{
			X + other.X,
			Y + other.Y,
			Z + other.Z
		};
	}

	ChunkCoords& operator+=(const ChunkCoords& other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;
		return *this;
	}

	constexpr ChunkCoords operator-(const ChunkCoords& other) const
	{
		return
		{
			X - other.X,
			Y - other.Y,
			Z - other.Z
		};
	}

	ChunkCoords& operator-=(const ChunkCoords& other)
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
		return *this;
	}

	constexpr ChunkCoords operator*(int k)
	{
		return
		{
			X * k,
			Y * k,
			Z * k
		};
	}

	ChunkCoords& operator*=(int k)
	{
		X *= k;
		Y *= k;
		Z *= k;
		return *this;
	}
};

namespace std
{
	template <>
	struct hash<ChunkCoords>
	{
		size_t operator()(const ChunkCoords& coords) const
		{
			size_t h1 = std::hash<int>{}(coords.X);
			size_t h2 = std::hash<int>{}(coords.Y);
			size_t h3 = std::hash<int>{}(coords.Z);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	template <>
	struct hash<ChunkCoords2D>
	{
		size_t operator()(const ChunkCoords2D& coords) const
		{
			size_t h1 = std::hash<int>{}(coords.X);
			size_t h2 = std::hash<int>{}(coords.Z);
			return h1 ^ (h2 << 1);
		}
	};
}

struct LocalBlockCoords
{
	uint8_t X = 0;
	uint8_t Y = 0;
	uint8_t Z = 0;

	size_t ToIndex() const 
	{ 
		return static_cast<size_t>(X) | static_cast<size_t>(Z) << 4u | static_cast<size_t>(Y) << 8u; 
	}
};

struct LocalBlockCoords2D
{
	uint8_t X = 0;
	uint8_t Z = 0;

	size_t ToIndex() const
	{
		return static_cast<size_t>(X) | static_cast<size_t>(Z) << 4u;
	}
};

struct BlockCoords
{
	int X = 0;
	int Y = 0;
	int Z = 0;

	explicit operator ChunkCoords() const
	{
		return
		{
			X >= 0 ? X / 16 : (X - 15) / 16,
			Y >= 0 ? Y / 16 : (Y - 15) / 16,
			Z >= 0 ? Z / 16 : (Z - 15) / 16
		};
	}

	explicit operator ChunkCoords2D() const
	{
		return
		{
			X >= 0 ? X / 16 : (X - 15) / 16,
			Z >= 0 ? Z / 16 : (Z - 15) / 16
		};
	}

	explicit operator LocalBlockCoords() const
	{
		return
		{
			static_cast<uint8_t>(X & 0xF),
			static_cast<uint8_t>(Y & 0xF),
			static_cast<uint8_t>(Z & 0xF)
		};
	}

	constexpr BlockCoords operator+(const BlockCoords& other) const
	{
		return
		{
			X + other.X,
			Y + other.Y,
			Z + other.Z
		};
	}

	BlockCoords& operator+=(const BlockCoords& other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;
		return *this;
	}

	constexpr BlockCoords operator-(const BlockCoords& other) const
	{
		return
		{
			X - other.X,
			Y - other.Y,
			Z - other.Z
		};
	}

	BlockCoords& operator-=(const BlockCoords& other)
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
		return *this;
	}

	constexpr BlockCoords operator*(int k)
	{
		return
		{
			X * k,
			Y * k,
			Z * k
		};
	}

	BlockCoords& operator*=(int k)
	{
		X *= k;
		Y *= k;
		Z *= k;
		return *this;
	}
};

struct WorldCoords
{
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;

	static WorldCoords Lerp(const WorldCoords& c1, const WorldCoords& c2, float alpha)
	{
		return
		{
			c1.X * (1 - alpha) + c2.X * alpha,
			c1.Y * (1 - alpha) + c2.Y * alpha,
			c1.Z * (1 - alpha) + c2.Z * alpha
		};
	}

	float Length() const
	{
		return std::sqrt(X * X + Y * Y + Z * Z);
	}

	explicit operator BlockCoords() const
	{
		return
		{
			static_cast<int>(std::floor(X)),
			static_cast<int>(std::floor(Y)),
			static_cast<int>(std::floor(Z))
		};
	}

	explicit operator ChunkCoords() const
	{
		return
		{
			static_cast<int>(std::floor(X)) / 16,
			static_cast<int>(std::floor(Y)) / 16,
			static_cast<int>(std::floor(Z)) / 16
		};
	}

	explicit operator glm::vec3() const
	{
		return
		{
			X,
			Y,
			Z
		};
	}

	constexpr WorldCoords operator+(const WorldCoords& other) const
	{
		return
		{
			X + other.X,
			Y + other.Y,
			Z + other.Z
		};
	}

	WorldCoords& operator+=(const WorldCoords& other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;
		return *this;
	}

	constexpr WorldCoords operator-(const WorldCoords& other) const
	{
		return
		{
			X - other.X,
			Y - other.Y,
			Z - other.Z
		};
	}

	WorldCoords& operator-=(const WorldCoords& other)
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
		return *this;
	}

	constexpr WorldCoords operator*(float k) const
	{
		return
		{
			X * k,
			Y * k,
			Z * k
		};
	}

	constexpr WorldCoords operator/(float k) const
	{
		return
		{
			X / k,
			Y / k,
			Z / k
		};
	}

	friend constexpr WorldCoords operator*(float k, WorldCoords coords)
	{
		return
		{
			k * coords.X,
			k * coords.Y,
			k * coords.Z
		};
	}

	WorldCoords& operator*=(float k)
	{
		X *= k;
		Y *= k;
		Z *= k;
		return *this;
	}
};

constexpr BlockCoords operator+(BlockCoords block, LocalBlockCoords local)
{
	return
	{
		block.X + local.X,
		block.Y + local.Y,
		block.Z + local.Z
	};
}

constexpr BlockCoords operator+(LocalBlockCoords local, BlockCoords block)
{
	return
	{
		local.X + block.X,
		local.Y + block.Y,
		local.Z + block.Z
	};
}