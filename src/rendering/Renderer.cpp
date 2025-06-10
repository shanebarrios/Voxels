#include "Renderer.h"

#include "Camera.h"
#include "../world/World.h"
#include <array>

Renderer::Renderer()
{
	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glLineWidth(2.0f);
	glEnable(GL_LINE_SMOOTH);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::Render(const World& world, const Camera& camera) const
{
	m_MatrixUBO.SetData(0u, sizeof(glm::mat4), glm::value_ptr(camera.GetProjectionMatrix()));
	m_MatrixUBO.SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));

	m_ChunkRenderer.Prepare();
	for (auto& [pos, chunk] : world.GetLoadedChunks())
	{
		if (chunk)
		{
			m_ChunkRenderer.Render(*chunk);
		}
	}
	m_BlockOutlineRenderer.Render(world, camera);
	m_CrosshairRenderer.Render();
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}