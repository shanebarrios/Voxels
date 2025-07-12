#pragma once

#include "ChunkRenderer.h"
#include "Buffer.h"
#include "Framebuffer.h"
#include <glm/glm.hpp>
#include "BlockOutlineRenderer.h"
#include "CrosshairRenderer.h"

class World;
class Camera;

class Renderer
{
public:
	Renderer(int windowWidth, int windowHeight);

	void Render(const World& world, const Camera& camera) const;

	void Clear() const;
private:
	int m_WindowWidth;
	int m_WindowHeight;

	UniformBuffer m_MatrixUBO{7 * sizeof(glm::mat4)};	
	Framebuffer m_ShadowFramebuffer{ 2048, 2048 };
	Framebuffer m_DeferredFramebuffer;
	ChunkRenderer m_ChunkRenderer{ m_MatrixUBO };
	BlockOutlineRenderer m_BlockOutlineRenderer{ m_MatrixUBO };
	CrosshairRenderer m_CrosshairRenderer{};
};