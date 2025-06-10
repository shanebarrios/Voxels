#include "CrosshairRenderer.h"

#include <array>

constexpr std::array k_CrosshairVertices
{
	-0.05f, -0.05f, 0.0f, 0.0f,
	-0.05f,  0.05f, 0.0f, 1.0f,
	 0.05f,  0.05f, 1.0f, 1.0f,
	-0.05f, -0.05f, 0.0f, 0.0f,
	 0.05f, -0.05f, 1.0f, 0.0f,
	 0.05f,  0.05f, 1.0f, 1.0f
};

CrosshairRenderer::CrosshairRenderer() :
	m_Shader{ ASSETS_PATH "shaders/crosshair.vert", ASSETS_PATH "shaders/crosshair.frag"},
	m_Texture{ ASSETS_PATH "textures/crosshair.png"}
{
	m_VBO.SetData(k_CrosshairVertices.data(), k_CrosshairVertices.size());
	m_VAO.SetVertexBuffer(m_VBO, BufferLayout{ {LayoutElementType::Float, 2}, {LayoutElementType::Float, 2} });
}

void CrosshairRenderer::Render() const
{
	glDisable(GL_CULL_FACE);
	m_Shader.Bind();
	m_Texture.Bind();
	m_VAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
}