#include <array>
#include <cmath>

struct Vec2
{
    float X;
    float Y;

    static constexpr float Dot(Vec2 v1, Vec2 v2) { return v1.X * v2.X + v1.Y * v2.Y; }

    constexpr Vec2 operator+(Vec2 other) const { return { X + other.X, Y + other.Y }; }
    constexpr Vec2 operator-(Vec2 other) const { return { X - other.X, Y - other.Y }; }
    constexpr Vec2 operator*(float k) const { return { X * k, Y * k }; }

    Vec2& operator+=(Vec2 other) { X += other.X; Y += other.Y; return *this; }
    Vec2& operator-=(Vec2 other) { X -= other.X; Y -= other.Y; return *this; }
    Vec2& operator*=(float k) { X *= k; Y *= k; return *this; }
};

static constexpr std::array<uint8_t, 256> k_Permutation = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static constexpr std::array<uint8_t, 512> RepeatPermutation()
{
    std::array<uint8_t, 512> ret;
    for (size_t i = 0; i < 512; i++)
    {
        ret[i] = k_Permutation[i & 0xFFu];
    }
    return ret;
}

static constexpr float Fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static constexpr std::array<Vec2, 8> k_Gradients =
{
    Vec2 { 1.0f,  0.0f},
    Vec2 {-1.0f,  0.0f},
    Vec2 { 0.0f,  1.0f},
    Vec2 { 0.0f, -1.0f},
    Vec2 { 1.0f,  1.0f},
    Vec2 {-1.0f,  1.0f},
    Vec2 { 1.0f, -1.0f},
    Vec2 {-1.0f, -1.0f}
};

static constexpr Vec2 GetRandomGradient(uint8_t hash)
{
    return k_Gradients[hash & 0x7u];
}

static constexpr float Lerp(float begin, float end, float alpha)
{
    return begin + (end - begin) * alpha;
}

namespace Noise
{
    float PerlinNoise(float x, float y)
    {
        static constexpr std::array<uint8_t, 512> permutation = RepeatPermutation();

        const int xi = static_cast<int>(std::floor(x)) & 0xFFu;
        const int yi = static_cast<int>(std::floor(y)) & 0xFFu;

        const float xf = x - std::floor(x);
        const float yf = y - std::floor(y);

        const Vec2 distTopLeft{ xf, yf - 1.0f };
        const Vec2 distTopRight{ xf - 1.0f, yf - 1.0f };
        const Vec2 distBottomLeft{ xf, yf };
        const Vec2 distBottomRight{ xf - 1.0f, yf };

        const Vec2 gradTopLeft = GetRandomGradient(permutation[permutation[xi] + yi + 1]);
        const Vec2 gradTopRight = GetRandomGradient(permutation[permutation[xi + 1] + yi + 1]);
        const Vec2 gradBottomLeft = GetRandomGradient(permutation[permutation[xi] + yi]);
        const Vec2 gradBottomRight = GetRandomGradient(permutation[permutation[xi + 1] + yi]);

        const float u = Fade(xf);
        const float v = Fade(yf);

        const float influence1 = Vec2::Dot(distTopLeft, gradTopLeft);
        const float influence2 = Vec2::Dot(distTopRight, gradTopRight);
        const float influence3 = Vec2::Dot(distBottomLeft, gradBottomLeft);
        const float influence4 = Vec2::Dot(distBottomRight, gradBottomRight);

        float ret =  Lerp(Lerp(influence3, influence4, u), Lerp(influence1, influence2, u), v);
        return ret;
    }

}
