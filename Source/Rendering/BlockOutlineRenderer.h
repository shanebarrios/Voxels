#pragma once

#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "World/Coordinates.h"

class World;
class Camera;

class BlockOutlineRenderer
{
public:
	explicit BlockOutlineRenderer(const UniformBuffer& cameraUBO);

	void Render(const World& world, const Camera& camera) const;

private:
	Shader m_Shader;
	VertexBuffer m_VBO{};
	VertexArray m_VAO{};
};