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
	m_DepthShader{ASSETS_PATH "Shaders/ChunkDepth.vert", ASSETS_PATH "Shaders/ChunkDepth.frag", ASSETS_PATH "Shaders/ChunkDepth.geom"},
	m_GBufferShader{ASSETS_PATH "Shaders/ChunkGBuffer.vert", ASSETS_PATH "Shaders/ChunkGBuffer.frag"},
	m_WaterShader{ASSETS_PATH "Shaders/Water.vert", ASSETS_PATH "Shaders/Water.frag"}
{
	m_DepthShader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
	m_GBufferShader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
	m_WaterShader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
}

void ChunkRenderer::RenderGBuffer(const std::vector<const Chunk*>& chunkList) const
{
	m_GBufferShader.Bind();
	m_TextureAtlas.Bind();

	for (const Chunk* chunk : chunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		if (mesh.NumOpaqueVertices() == 0) continue;
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_GBufferShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
	}
}

void ChunkRenderer::RenderDepth(const std::vector<const Chunk*>& chunkList) const
{
	m_DepthShader.Bind();
	m_TextureAtlas.Bind();
	
	for (const Chunk* chunk : chunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
	}
}

void ChunkRenderer::RenderWater(const std::vector<const Chunk*>& chunkList) const
{
	m_WaterShader.Bind();
	m_TextureAtlas.Bind();

	for (const Chunk* chunk : chunkList | std::views::reverse)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindTransparent();
		const ChunkCoords coords = chunk->GetCoords();
		m_GBufferShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
	}
}