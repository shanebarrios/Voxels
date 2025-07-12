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

	void RenderDepth(const std::vector<const Chunk*>& chunkList) const;
	void Render(const std::vector<const Chunk*>& chunkList, uint32_t lightDepthMaps, const Camera& camera) const;
private:
	Texture2D m_TextureAtlas;
	Shader m_Shader;
	Shader m_DepthShader;
};