#pragma once

#include "ChunkRenderer.h"
#include "Buffer.h"
#include <glm/glm.hpp>
#include "BlockOutlineRenderer.h"
#include "CrosshairRenderer.h"

class World;
class Camera;

class Renderer
{
public:
	Renderer();

	void Render(const World& world, const Camera& camera) const;

	void Clear() const;
private:
	UniformBuffer m_MatrixUBO{3 * sizeof(glm::mat4)};	
	ChunkRenderer m_ChunkRenderer{ m_MatrixUBO };
	BlockOutlineRenderer m_BlockOutlineRenderer{ m_MatrixUBO };
	CrosshairRenderer m_CrosshairRenderer{};
	uint32_t m_DepthMapFBO;
	Texture2D m_DepthMap{ Texture2D::Attachment(1024, 1024, Texture2D::Format::Depth)};
};