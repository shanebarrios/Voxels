#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Buffer.h"
#include "VertexArray.h"

#include <glm/glm.hpp>

class CrosshairRenderer
{
  public:
    CrosshairRenderer(int screenWidth, int screenHeight);

    void Render() const;

  private:
    VertexArray m_VAO{};
    VertexBuffer m_VBO{};
    Texture2D m_Texture;
    Shader m_Shader;

    glm::mat4 m_Transform;
};