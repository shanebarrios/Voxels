#pragma once

#include <limits>

namespace Noise
{
struct OctaveConfig
{
    int NumOctaves;
    float Persistence;
    float Lacunarity;
    float BaseFrequency;
};

float PerlinNoise(float x, float y);

class OctavePerlinNoise
{
  public:
    explicit OctavePerlinNoise(OctaveConfig config, int seed = 0);

    float Sample(float x, float y) const;

  private:
    void EstimateMinMax();

  private:
    OctaveConfig m_Config;
    float m_MinEstimate = std::numeric_limits<float>::max();
    float m_MaxEstimate = std::numeric_limits<float>::lowest();
};
} // namespace Noise