#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Buffer.h"
#include "VertexArray.h"

class CrosshairRenderer
{
public:
	CrosshairRenderer();

	void Render() const;
private:
	VertexArray m_VAO{};
	VertexBuffer m_VBO{};
	Texture2D m_Texture;
	Shader m_Shader;
};