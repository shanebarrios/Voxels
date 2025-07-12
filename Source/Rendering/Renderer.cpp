#include "Renderer.h"

#include "Camera.h"
#include "World/World.h"
#include "Utils/Logger.h"
#include <array>
#include <limits>

using SubfrustumCorners = std::array<glm::vec3, 8>;
using SubfrustaCorners = std::array<SubfrustumCorners, Camera::k_NumSubdivisions>;

static constexpr int k_ShadowResolution = 4096;

template <typename T>
using SubfrustumArray = std::array<T, Camera::k_NumSubdivisions>;

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

//static std::vector<const Chunk*> GetChunkDepthRenderList(
//	const std::vector<const Chunk*>& renderableChunks,
//	const SubfrustumArray<AABB>& aabbs,
//	const SubfrustumArray<glm::mat4>& viewMatrices)
//{
//	std::vector<const Chunk*> ret{};
//	ret.reserve(renderableChunks.size() / 4);
//	for (const Chunk* chunk : renderableChunks)
//	{
//		for (size_t i = 0; i < aabbs.size(); i++)
//		{
//
//		}
//		const AABB chunkAABB = GetChunkAABB(chunk->GetCoords());
//		for (const AABB& subfrustaAABB : subfrustaAABBs)
//		{
//			if (CheckAABBIntersection(chunkAABB, subfrustaAABB))
//			{
//				ret.push_back(chunk);
//				break;
//			}
//		}
//	}
//	return ret;
//}

Renderer::Renderer(int windowWidth, int windowHeight) :
	m_WindowWidth{ windowWidth }, m_WindowHeight{ windowHeight },
	m_DeferredFramebuffer{ windowWidth, windowHeight } 
{
	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0f);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	const FramebufferAttachment depthMapAttachment
	{
		.Format = FramebufferTextureFormat::Depth32F,
		.Type = FramebufferTextureType::Texture2DArray,
		.LayerCount = Camera::k_NumSubdivisions
	};
	m_ShadowFramebuffer.SetAttachments({ depthMapAttachment });

	const FramebufferAttachment positionAttachment
	{
		.Format = FramebufferTextureFormat::RGBA16F
	};

	const FramebufferAttachment normalAttachment
	{
		.Format = FramebufferTextureFormat::RGBA16F
	};

	const FramebufferAttachment colorAttachment
	{
		.Format = FramebufferTextureFormat::RGBA8
	};

	m_DeferredFramebuffer.SetAttachments({ positionAttachment, normalAttachment, colorAttachment });
}

void Renderer::Render(const World& world, const Camera& camera) const
{
	static const glm::vec3 lightDir = glm::normalize(glm::vec3{ 0.6, -0.7, 0.2 });

	const glm::mat4& view = camera.GetViewMatrix();
	SubfrustumArray<AABB> subfrustaAABBs;
	SubfrustumArray<glm::mat4> lightViewMatrices;

	for (int i = 0; i < Camera::k_NumSubdivisions; i++)
	{
		const glm::mat4& proj = camera.GetSubfrustaProjectionMatrix(i);
		SubfrustumCorners corners;
		camera.GetSubfrustumCornersWorldSpace(corners, i);
		const glm::vec3 frustumMidpoint = GetFrustumMidpoint(corners);
		lightViewMatrices[i] = glm::lookAt(frustumMidpoint - lightDir, frustumMidpoint, glm::vec3{0.0f, 1.0f, 0.0f});
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

	std::vector<const Chunk*> chunkDepthRenderList = world.GetChunkRenderList();
	std::vector<const Chunk*> chunkRenderList = GetChunkRenderList(chunkDepthRenderList, camera);

	m_ShadowFramebuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);

	m_ChunkRenderer.RenderDepth(chunkRenderList);

	m_ShadowFramebuffer.Unbind(2560, 1440);
	glEnable(GL_BLEND);

	m_ChunkRenderer.Render(chunkRenderList, m_ShadowFramebuffer.GetDepthAttachment(), camera);
	m_BlockOutlineRenderer.Render(world, camera);
	m_CrosshairRenderer.Render();
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}