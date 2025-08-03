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
private:
	int m_WindowWidth;
	int m_WindowHeight;

	UniformBuffer m_MatrixUBO{7 * sizeof(glm::mat4)};	

	Framebuffer m_ShadowFramebuffer{ 4096, 4096 };
	Framebuffer m_DeferredFramebuffer{ m_WindowWidth, m_WindowHeight };

	Shader m_QuadShader{ASSETS_PATH "Shaders/Quad.vert", ASSETS_PATH "Shaders/Quad.frag"};
	Shader m_DeferredLightingShader{ ASSETS_PATH "Shaders/Quad.vert", ASSETS_PATH "Shaders/DeferredLighting.frag" };

	VertexBuffer m_QuadVBO{};
	VertexArray m_QuadVAO{};

	ChunkRenderer m_ChunkRenderer{ m_MatrixUBO };
	BlockOutlineRenderer m_BlockOutlineRenderer{ m_MatrixUBO };
	CrosshairRenderer m_CrosshairRenderer{ m_WindowWidth, m_WindowHeight };

	void InitFramebuffers();

	void InitQuadData();

	void ConfigureMatrices(const Camera& camera) const;

	void RenderShadowPass(const std::vector<const Chunk*>& chunks, size_t cascade) const;

	void RenderGBufferPass(const std::vector<const Chunk*>& chunks) const;

	void RenderLightingPass(const Camera& camera) const;

	void RenderForwardPass(const World& world, const Camera& camera) const;

	void DrawFullScreenQuad(uint32_t textureId) const;
};