#include "ChunkRenderer.h"
#include <glad/glad.h>
#include <ranges>
#include "World/Chunk.h"
#include "World/Coordinates.h"
#include "Camera.h"
#include "Buffer.h"
#include "Core/DebugState.h"

extern DebugState g_DebugState;

ChunkRenderer::ChunkRenderer(const UniformBuffer& cameraUBO) :
	m_TextureAtlas{Texture2D::FromPath(ASSETS_PATH "Textures/VoxelTextures.png")},
	m_DepthShader{ASSETS_PATH "Shaders/ChunkDepth.vert", ASSETS_PATH "Shaders/ChunkDepth.frag"},
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
		m_GBufferShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * CHUNK_DIMENSION, coords.Y * CHUNK_DIMENSION, coords.Z * CHUNK_DIMENSION);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
		g_DebugState.DrawCalls++;
	}
}

void ChunkRenderer::RenderDepth(const std::vector<const Chunk*>& chunkList, size_t cascade) const
{
	m_DepthShader.Bind();
	m_TextureAtlas.Bind();

	m_DepthShader.SetUniform(Shader::UNIFORM_CASCADE_INDEX, static_cast<uint32_t>(cascade));
	
	for (const Chunk* chunk : chunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * CHUNK_DIMENSION, coords.Y * CHUNK_DIMENSION, coords.Z * CHUNK_DIMENSION);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
		g_DebugState.DrawCalls++;
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
		m_GBufferShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * CHUNK_DIMENSION, coords.Y * CHUNK_DIMENSION, coords.Z * CHUNK_DIMENSION);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
		g_DebugState.DrawCalls++;
	}
}