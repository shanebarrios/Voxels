#include "ChunkRenderer.h"
#include <glad/glad.h>
#include <ranges>
#include "World/Chunk.h"
#include "Coordinates.h"
#include "Camera.h"
#include "Buffer.h"

ChunkRenderer::ChunkRenderer(const UniformBuffer& cameraUBO) :
	m_TextureAtlas{Texture2D::FromPath(ASSETS_PATH "Textures/VoxelTextures.png")},
	m_Shader{ASSETS_PATH "Shaders/Chunk.vert", ASSETS_PATH "Shaders/Chunk.frag"},
	m_DepthShader{ASSETS_PATH "Shaders/ChunkDepth.vert", ASSETS_PATH "Shaders/ChunkDepth.frag"}
{
	m_Shader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
	m_DepthShader.BindUniformBlock(cameraUBO.GetBindingPoint(), "Matrices");
}

void ChunkRenderer::RenderDepth(const std::vector<const Chunk*>& opaqueChunkList,
	const std::vector<const Chunk*>& transparentChunkList) const
{
	m_DepthShader.Bind();
	m_TextureAtlas.Bind(0);

	for (const Chunk* chunk : opaqueChunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
	}
	for (const Chunk* chunk : transparentChunkList | std::views::reverse)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindTransparent();
		const ChunkCoords coords = chunk->GetCoords();
		m_DepthShader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
	}
}

void ChunkRenderer::Render(const std::vector<const Chunk*>& opaqueChunkList, 
	const std::vector<const Chunk*>& transparentChunkList, const Texture2D& depthMap) const
{
	m_Shader.Bind();
	m_TextureAtlas.Bind(0);
	depthMap.Bind(1);
	m_Shader.SetUniform(Shader::UNIFORM_TEXTURE_ATLAS, 0);
	m_Shader.SetUniform(Shader::UNIFORM_SHADOW_MAP, 1);

	for (const Chunk* chunk : opaqueChunkList)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindOpaque();
		const ChunkCoords coords = chunk->GetCoords();
		m_Shader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumOpaqueVertices()));
	}
	for (const Chunk* chunk : transparentChunkList | std::views::reverse)
	{
		const ChunkMesh& mesh = chunk->GetMesh();
		mesh.BindTransparent();
		const ChunkCoords coords = chunk->GetCoords();
		m_Shader.SetUniform(Shader::UNIFORM_POSITION, coords.X * 16, coords.Y * 16, coords.Z * 16);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.NumTransparentVertices()));
	}
}