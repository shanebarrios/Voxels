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
	m_Shader{ ASSETS_PATH "Shaders/Quad.vert", ASSETS_PATH "Shaders/Quad.frag"},
	m_Texture{ Texture2D::FromPath(ASSETS_PATH "Textures/Crosshair.png")}
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