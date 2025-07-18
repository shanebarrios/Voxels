#include "Renderer.h"

#include "Camera.h"
#include "World/World.h"
#include "Utils/Logger.h"
#include "Math/MathUtils.h"
#include <array>
#include <limits>
#include <random>

using SubfrustumCorners = std::array<glm::vec3, 8>;
using SubfrustaCorners = std::array<SubfrustumCorners, Camera::k_NumSubdivisions>;

template <typename T>
using SubfrustumArray = std::array<T, Camera::k_NumSubdivisions>;

static constexpr glm::mat4 k_Identity{ 1.0f };

struct AABB
{
	glm::vec3 Min;
	glm::vec3 Max;
};

static glm::vec3 GetFrustumMidpoint(const SubfrustumCorners& corners)
{
	glm::vec3 sum{ 0.0f };
	for (const glm::vec3& vec : corners)
	{
		sum += vec;
	}
	return sum / 8.0f;
}

static AABB GetLightAABBViewSpace(const SubfrustumCorners& corners, const glm::mat4& view)
{
	AABB aabb
	{ 
		.Min = glm::vec3{ std::numeric_limits<float>::max() }, 
		.Max = glm::vec3{ std::numeric_limits<float>::min()} 
	};
	for (const glm::vec3& vec : corners)
	{
		const glm::vec3 viewVec = view * glm::vec4{ vec, 1.0f };
		aabb.Min = glm::min(aabb.Min, viewVec);
		aabb.Max = glm::max(aabb.Max, viewVec);
	}

	aabb.Min.z < 0.0f ? aabb.Min.z *= 10.0f : aabb.Min.z /= 10.0f;
	aabb.Max.z < 0.0f ? aabb.Max.z /= 10.0f : aabb.Max.z *= 10.0f;

	return aabb;
}

static AABB GetSubfrustaAABB(const SubfrustumCorners& corners)
{
	glm::vec3 min{ std::numeric_limits<float>::max() };
	glm::vec3 max{ std::numeric_limits<float>::lowest() };

	for (const glm::vec3& vec : corners)
	{
		min = glm::min(min, vec);
		max = glm::max(max, vec);
	}

	const float padding = 32.0f;
	min -= glm::vec3{ padding };
	max += glm::vec3{ padding };

	return AABB{ min, max };
}

static AABB GetChunkAABB(ChunkCoords coords)
{
	const glm::vec3 min{ coords.X * 16, coords.Y * 16, coords.Z * 16 };
	const glm::vec3 max = min + glm::vec3{ 16.0f, 16.0f, 16.0f };
	return AABB{ min, max };
}

static bool IsInsidePlane(const glm::vec3& pos, const Plane& plane)
{
	return glm::dot(pos - plane.P0, plane.Normal) >= 0.0f;
}

static bool CheckAABBIntersection(AABB a, AABB b)
{
	return
		a.Min.x <= b.Max.x &&
		a.Max.x >= b.Min.x &&
		a.Min.y <= b.Max.y &&
		a.Max.y >= b.Min.y &&
		a.Min.z <= b.Max.z &&
		a.Max.z >= b.Min.z;
}

static bool ChunkInFrustrum(ChunkCoords coords, const std::array<Plane, 6>& frustumPlanes)
{
	const glm::vec3 chunkWorldPos
	{
		coords.X * 16,
		coords.Y * 16,
		coords.Z * 16
	};
	std::array<glm::vec3, 8> chunkCorners
	{
		chunkWorldPos,
		chunkWorldPos + glm::vec3{16.0f, 0.0f, 0.0f},
		chunkWorldPos + glm::vec3{0.0f, 16.0f, 0.0f},
		chunkWorldPos + glm::vec3{0.0f, 0.0f, 16.0f},
		chunkWorldPos + glm::vec3{16.0f, 16.0f, 0.0f},
		chunkWorldPos + glm::vec3{0.0f, 16.0f, 16.0f},
		chunkWorldPos + glm::vec3{16.0f, 0.0f, 16.0f},
		chunkWorldPos + glm::vec3{16.0f, 16.0f, 16.0f}
	};

	for (const Plane& plane : frustumPlanes)
	{
		bool allOutside = true;
		for (const glm::vec3& corner : chunkCorners)
		{
			if (IsInsidePlane(corner, plane))
			{
				allOutside = false;
				break;
			}
		}
		if (allOutside)
		{
			return false;
		}
	}
	return true;
}

static bool ChunkInLightView(ChunkCoords coords, const std::array<AABB, Camera::k_NumSubdivisions>& aabbs)
{
	const AABB chunkAABB = GetChunkAABB(coords);
	for (const AABB& aabb : aabbs)
	{
		if (CheckAABBIntersection(aabb, chunkAABB)) return true;
	}
	return false;
}

static std::vector<const Chunk*> GetChunkRenderList(
	const std::vector<const Chunk*>& renderableChunks,
	const Camera& camera)
{
	std::array<Plane, 6> frustumPlanes;
	camera.GetFrustumPlanes(frustumPlanes);

	std::vector<const Chunk*> ret{};
	ret.reserve(renderableChunks.size() / 4);
	for (const Chunk* chunk : renderableChunks)
	{
		if (ChunkInFrustrum(chunk->GetCoords(), frustumPlanes))
		{
			ret.push_back(chunk);
		}
	}
	return ret;
}

Renderer::Renderer(int windowWidth, int windowHeight) :
	m_WindowWidth{ windowWidth }, m_WindowHeight{ windowHeight }
{
	m_DeferredLightingShader.BindUniformBlock(m_MatrixUBO.GetBindingPoint(), "Matrices");
	m_SSAOShader.BindUniformBlock(m_MatrixUBO.GetBindingPoint(), "Matrices");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0f);
	glEnable(GL_LINE_SMOOTH);

	InitFramebuffers();

	InitSSAOData();

	InitQuadData();
}

void Renderer::InitFramebuffers()
{
	const FramebufferAttachment depthMapAttachment
	{
		.Format = FramebufferAttachmentFormat::Depth32F,
		.Type = FramebufferAttachmentType::Texture2DArray,
		.LayerCount = Camera::k_NumSubdivisions
	};
	m_ShadowFramebuffer.SetAttachments({ depthMapAttachment });

	const FramebufferAttachment positionAttachment{ FramebufferAttachmentFormat::RGBA32F };
	const FramebufferAttachment normalAttachment{ FramebufferAttachmentFormat::RGBA16F };
	const FramebufferAttachment albedoAttachment{ FramebufferAttachmentFormat::RGBA16F };
	const FramebufferAttachment depthAttachment
	{
		FramebufferAttachmentFormat::Depth24,
		FramebufferAttachmentType::Renderbuffer
	};

	m_DeferredFramebuffer.SetAttachments({ positionAttachment, normalAttachment, albedoAttachment, depthAttachment });

	const FramebufferAttachment ssaoColorAttachment{ FramebufferAttachmentFormat::R32F };
	m_SSAOFramebuffer.SetAttachments({ ssaoColorAttachment });

	m_SSAOBlurFramebuffer.SetAttachments({ ssaoColorAttachment });
}

void Renderer::InitSSAOData()
{
	std::uniform_real_distribution<float> randomFloats{ 0.0f, 1.0f };
	std::mt19937 mt;
	for (size_t i = 0; i < m_SSAOKernel.size(); i++)
	{
		glm::vec3 sample = glm::normalize(glm::vec3
			{
				randomFloats(mt) * 2.0f - 1.0f,
				randomFloats(mt) * 2.0f - 1.0f,
				randomFloats(mt)
			});
		const float ratio = static_cast<float>(i) / 64.0f;
		const float scale = MathUtils::Lerp(0.1f, 1.0f, ratio * ratio);
		sample *= scale;
		m_SSAOKernel[i] = sample;
	}

	std::array<glm::vec3, 16> ssaoNoise;
	for (size_t i = 0; i < 16; i++)
	{
		glm::vec3 noise
		{
			randomFloats(mt) * 2.0f - 1.0f,
			randomFloats(mt) * 2.0f - 1.0f,
			0.0f
		};
		ssaoNoise[i] = noise;
	}

	m_SSAORotationVectors = Texture2D::FromData(
		ssaoNoise.data(), 4, 4,
		TextureInternalFormat::RGBA16F,
		TextureWrap::Repeat
	);
}

void Renderer::InitQuadData()
{
	constexpr std::array k_QuadVertices
	{
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,

		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	m_QuadVBO.SetData(k_QuadVertices.data(), k_QuadVertices.size());
	const BufferLayout layout{ {LayoutElementType::Float, 2}, {LayoutElementType::Float, 2} };
	m_QuadVAO.SetVertexBuffer(m_QuadVBO, layout);
}

void Renderer::Render(const World& world, const Camera& camera) const
{
	ConfigureMatrices(camera);

	std::vector<const Chunk*> chunkRenderList = GetChunkRenderList(world.GetChunkRenderList(), camera);

	RenderShadowPass(chunkRenderList);

	RenderGBufferPass(chunkRenderList);

	RenderSSAOPass();
	
	RenderLightingPass(camera);

	RenderForwardPass(world, camera);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

static const glm::vec3 lightDir = glm::normalize(glm::vec3{ 0.6, -0.7, 0.2 });

void Renderer::ConfigureMatrices(const Camera& camera) const
{
	const glm::mat4& view = camera.GetViewMatrix();
	SubfrustumArray<AABB> subfrustaAABBs;
	SubfrustumArray<glm::mat4> lightViewMatrices;

	for (int i = 0; i < Camera::k_NumSubdivisions; i++)
	{
		const glm::mat4& proj = camera.GetSubfrustaProjectionMatrix(i);
		SubfrustumCorners corners;
		camera.GetSubfrustumCornersWorldSpace(corners, i);
		const glm::vec3 frustumMidpoint = GetFrustumMidpoint(corners);
		lightViewMatrices[i] = glm::lookAt(frustumMidpoint - lightDir, frustumMidpoint, glm::vec3{ 0.0f, 1.0f, 0.0f });
		const AABB lightAABB = GetLightAABBViewSpace(corners, lightViewMatrices[i]);
		subfrustaAABBs[i] = lightAABB;
		const glm::mat4 lightProj = glm::ortho(
			lightAABB.Min.x, lightAABB.Max.x,
			lightAABB.Min.y, lightAABB.Max.y,
			lightAABB.Min.z, lightAABB.Max.z
		);
		const glm::mat4 lightProjView = lightProj * lightViewMatrices[i];
		m_MatrixUBO.SetData((i + 2) * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightProjView));
	}

	m_MatrixUBO.SetData(0u, sizeof(glm::mat4), glm::value_ptr(camera.GetProjectionMatrix()));
	m_MatrixUBO.SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
}

void Renderer::RenderShadowPass(const std::vector<const Chunk*>& chunkList) const
{
	m_ShadowFramebuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	m_ChunkRenderer.RenderDepth(chunkList);
}

void Renderer::RenderGBufferPass(const std::vector<const Chunk*>& chunkList) const
{
	m_DeferredFramebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_ChunkRenderer.RenderGBuffer(chunkList);
}

void Renderer::RenderLightingPass(const Camera& camera) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	glClear(GL_COLOR_BUFFER_BIT);

	m_DeferredLightingShader.Bind();
	for (size_t i = 0; i < 3; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_DeferredFramebuffer.GetTextureAttachment(i));
	}
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_SSAOBlurFramebuffer.GetTextureAttachment(0));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowFramebuffer.GetTextureAttachment(0));

	const glm::vec3 lightDirViewSpace = glm::normalize(camera.GetViewMatrix() * glm::vec4{ lightDir, 0.0 });
	m_DeferredLightingShader.SetUniform(Shader::UNIFORM_LIGHT_DIR, lightDirViewSpace);

	const auto& depths = camera.GetSubfrustaPlaneDepths();

	m_DeferredLightingShader.SetUniform(Shader::UNIFORM_SUBFRUSTA_PLANES, depths[1], depths[2], depths[3], depths[4]);

	m_QuadVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::RenderSSAOPass() const
{
	m_SSAOFramebuffer.Bind();
	m_SSAOShader.Bind();
	m_QuadVAO.Bind();

	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DeferredFramebuffer.GetTextureAttachment(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_DeferredFramebuffer.GetTextureAttachment(1));
	m_SSAORotationVectors.Bind(2);

	m_SSAOShader.SetUniform(Shader::UNIFORM_SAMPLES, m_SSAOKernel.data(), m_SSAOKernel.size());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_SSAOBlurFramebuffer.Bind();
	m_SSAOBlurShader.Bind();
	m_QuadVAO.Bind();
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SSAOFramebuffer.GetTextureAttachment(0));
	m_SSAOBlurShader.SetUniform(Shader::UNIFORM_TRANSFORM, k_Identity);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::RenderForwardPass(const World& world, const Camera& camera) const
{
	const std::vector<const Chunk*> waterChunks = GetChunkRenderList(world.GetChunkWaterRenderList(), camera);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_DeferredFramebuffer.GetId());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_WindowWidth, m_WindowHeight, 0, 0, m_WindowWidth, m_WindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	m_ChunkRenderer.RenderWater(waterChunks);

	m_BlockOutlineRenderer.Render(world, camera);
	glDisable(GL_DEPTH_TEST);
	m_CrosshairRenderer.Render();
}

void Renderer::DrawFullScreenQuad(uint32_t textureId) const
{
	m_QuadShader.Bind();
	m_QuadVAO.Bind();
	m_QuadShader.SetUniform(Shader::UNIFORM_TRANSFORM, k_Identity);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}