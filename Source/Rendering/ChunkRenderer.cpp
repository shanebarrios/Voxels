#include "ChunkRenderer.h"
#include <glad/glad.h>
#include <ranges>
#include "World/Chunk.h"
#include "Utils/Coordinates.h"
#include "Camera.h"
#include "Buffer.h"

int drawCalls = 0;

ChunkRenderer::ChunkRenderer(const UniformBuffer& cameraUBO) :
	m_TextureAtlas{Texture2D::FromPath(ASSETS_PATH "Textures/VoxelTextures.png")},
	m_Shader{ASSETS_PATH "Shaders/Chunk.vert", ASSETS_PATH "Shaders/Chunk.frag"},
	m_DepthShader{ASSETS_PATH "Shaders/ChunkDepth.vert", ASSETS_PATH "Shaders/ChunkDepth.frag", ASSETS_PATH "Shaders/ChunkDepth.geom"}
{
	m_Shader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
	m_DepthShader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
}

void ChunkRenderer::RenderDepth(const std::vector<const Chunk*>& chunkList) const
{
	m_DepthShader.Bind();
	m_TextureAtlas.Bind();
	
	for (const Chunk* chunk : chunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumOpaqueVertices() == 0) continue;
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
	}
	for (const Chunk* chunk : chunkList | std::views::reverse)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumTransparentVertices() == 0) continue;
		mesh.BindTransparent();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
	}
}

void ChunkRenderer::Render(const std::vector<const Chunk*>& chunkList, uint32_t lightDepthMaps, const Camera& camera) const
{
	m_Shader.Bind();
	m_TextureAtlas.Bind(0);
	m_Shader.SetUniform(Shader::UNIFORM_TEXTURE_ATLAS, 0);
	m_Shader.SetUniform(Shader::UNIFORM_SHADOW_MAP, 1);
	const auto& depths = camera.GetSubfrustaPlaneDepths();
	m_Shader.SetUniform(Shader::UNIFORM_SUBFRUSTA_PLANES, depths[1], depths[2], depths[3], depths[4]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);

	for (const Chunk* chunk : chunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumOpaqueVertices() == 0) continue;
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_Shader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
		drawCalls++;
	}
	glDisable(GL_CULL_FACE);
	for (const Chunk* chunk : chunkList | std::views::reverse)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumTransparentVertices() == 0) continue;
		mesh.BindTransparent();
		const ChunkCoords coords = chunk->GetCoords();
		m_Shader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
		drawCalls++;
	}
	glEnable(GL_CULL_FACE);
}