#pragma once

#include "Texture.h"
#include "Shader.h"

class Chunk;
class Camera;
class UniformBuffer;

class ChunkRenderer
{
public:
	explicit ChunkRenderer(const UniformBuffer& cameraUBO);

	void Prepare() const;
	void Render(const Chunk& chunk) const;
private:
	Texture2D m_TextureAtlas;
	Shader m_Shader;
};