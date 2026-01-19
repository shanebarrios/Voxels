#pragma once

#include "Texture.h"
#include "Shader.h"
#include <vector>

class Chunk;
class Camera;
class UniformBuffer;

class ChunkRenderer
{
  public:
    explicit ChunkRenderer(const UniformBuffer& cameraUBO);

    void RenderGBuffer(const std::vector<const Chunk*>& chunkList) const;
    void RenderDepth(const std::vector<const Chunk*>& chunkList,
                     size_t cascade) const;
    void RenderWater(const std::vector<const Chunk*>& chunkList) const;

  private:
    Texture2D m_TextureAtlas;
    Shader m_GBufferShader;
    Shader m_DepthShader;
    Shader m_WaterShader;
};