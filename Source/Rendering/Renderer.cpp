#include "Renderer.h"

#include "Camera.h"
#include "World/World.h"
#include <array>

//constexpr std::array k_QuadVertices
//{
//	-1.0f, -1.0f, 0.0f, 0.0f,
//	-1.0f,  1.0f, 0.0f, 1.0f,
//	 1.0f,  1.0f, 1.0f, 1.0f,
//	-1.0f, -1.0f, 0.0f, 0.0f,
//	 1.0f, -1.0f, 1.0f, 0.0f,
//	 1.0f,  1.05f, 1.0f, 1.0f
//};

Renderer::Renderer()
{
	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0f);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	uint32_t depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	//uint32_t depthMap;
	//glGenTextures(1, &depthMap);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap.GetId(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_DepthMapFBO = depthMapFBO;
}

void Renderer::Render(const World& world, const Camera& camera) const
{
	static const glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 100.0f);
	static const glm::vec3 lightPos{ 60.0f, 75.0f, -57.0f };
	static const glm::vec3 lightLook{ 18.7f, 44.6f, -56.2f };
	static const glm::mat4 lightView = glm::lookAt(lightPos,
		lightLook, glm::vec3{ 0.0f, 1.0f, 0.0f });
	static const glm::mat4 lightSpace = lightProjection * lightView;

	m_MatrixUBO.SetData(0u, sizeof(glm::mat4), glm::value_ptr(camera.GetProjectionMatrix()));
	m_MatrixUBO.SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
	m_MatrixUBO.SetData(2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpace));

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	m_ChunkRenderer.RenderDepth(world.GetOpaqueChunkRenderList(), world.GetTransparentChunkRenderList());

	glViewport(0, 0, 2560, 1440);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	m_ChunkRenderer.Render(world.GetOpaqueChunkRenderList(), world.GetTransparentChunkRenderList(), m_DepthMap);
	m_BlockOutlineRenderer.Render(world, camera);
	m_CrosshairRenderer.Render();
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}