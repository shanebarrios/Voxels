#include "ChunkRenderer.h"
#include <glad/glad.h>
#include "../world/Chunk.h"
#include "../Coordinates.h"
#include "Camera.h"
#include "Buffer.h"

ChunkRenderer::ChunkRenderer(const UniformBuffer& cameraUBO) :
	m_TextureAtlas{ASSETS_PATH "textures/voxel_textures.png"},
	m_Shader{ASSETS_PATH "shaders/chunk.vert", ASSETS_PATH "shaders/chunk.frag"}
{
	m_Shader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
}

void ChunkRenderer::Prepare() const
{
	m_Shader.Bind();
	m_TextureAtlas.Bind();
}

void ChunkRenderer::Render(const Chunk& chunk) const
{
	const ChunkMesh& mesh = chunk.GetMesh();
	// this one line is an insane fps boost
	if (mesh.NumVertices() == 0) return;

	mesh.Bind();
	ChunkCoords coords = chunk.GetCoords();
	m_Shader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumVertices()));
}