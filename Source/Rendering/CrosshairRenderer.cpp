#include "CrosshairRenderer.h"

#include <array>

constexpr std::array k_CrosshairVertices
{
	-0.025f, -0.025f, 0.0f, 0.0f,
	-0.025f,  0.025f, 0.0f, 1.0f,
	 0.025f,  0.025f, 1.0f, 1.0f,
	-0.025f, -0.025f, 0.0f, 0.0f,
	 0.025f, -0.025f, 1.0f, 0.0f,
	 0.025f,  0.025f, 1.0f, 1.0f
};

CrosshairRenderer::CrosshairRenderer(int screenWidth, int screenHeight) :
	m_Shader{ ASSETS_PATH "Shaders/Quad.vert", ASSETS_PATH "Shaders/Quad.frag" },
	m_Texture{ Texture2D::FromPath(ASSETS_PATH "Textures/Crosshair.png") },
	m_Transform
	{
		glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f) *
		glm::scale(glm::mat4{1.0f}, glm::vec3{static_cast<float>(screenHeight) / screenWidth, 1.0f, 1.0f}) 
	}
{
	m_VBO.SetData(k_CrosshairVertices.data(), k_CrosshairVertices.size());
	m_VAO.SetVertexBuffer(m_VBO, BufferLayout{ {LayoutElementType::Float, 2}, {LayoutElementType::Float, 2} });
}

void CrosshairRenderer::Render() const
{
	m_Shader.Bind();
	m_Texture.Bind();
	m_VAO.Bind();
	m_Shader.SetUniform(Shader::UNIFORM_TRANSFORM, m_Transform);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}