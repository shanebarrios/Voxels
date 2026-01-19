#include "BlockOutlineRenderer.h"
#include <array>
#include <cstdint>
#include "Physics/Raycast.h"
#include "Camera.h"
#include "Core/Logger.h"

static constexpr std::array<int, 72> k_OutlineVertices{
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,

    1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0,

    1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1,

    0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};

BlockOutlineRenderer::BlockOutlineRenderer(const UniformBuffer& cameraUBO)
    : m_Shader{ASSETS_PATH "Shaders/Outline.vert",
               ASSETS_PATH "Shaders/Outline.frag"}
{
    m_VBO.SetData(k_OutlineVertices.data(), k_OutlineVertices.size());
    m_VAO.SetVertexBuffer(m_VBO, BufferLayout{{LayoutElementType::Int, 3}});
    m_Shader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
}

void BlockOutlineRenderer::Render(const World& world,
                                  const Camera& camera) const
{
    const glm::vec3 cameraPos = camera.GetPosition();
    const glm::vec3 cameraDir = camera.GetDirection();

    const WorldCoords rayStart = {cameraPos.x, cameraPos.y, cameraPos.z};
    const WorldCoords rayDirection = {cameraDir.x, cameraDir.y, cameraDir.z};

    const Raycast ray{rayStart, rayDirection, 6.0f};

    std::optional<Raycast::RaycastHit> result = ray.Cast(world);

    if (result)
    {

        const BlockCoords blockCoords = result->Coords;
        m_VAO.Bind();
        m_Shader.Bind();
        m_Shader.SetUniform(Shader::UNIFORM_POSITION, blockCoords.X,
                            blockCoords.Y, blockCoords.Z);
        glDrawArrays(GL_LINES, 0, k_OutlineVertices.size() / 3);
    }
}