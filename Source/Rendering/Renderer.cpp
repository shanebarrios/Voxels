#include "Renderer.h"

#include "Camera.h"
#include "World/World.h"
#include "Logger.h"
#include <array>
#include <limits>

static constexpr std::array k_CrosshairVertices
{
	-1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 1.0f, 1.0f
};

static void GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view, std::array<glm::vec3, 8>& corners)
{
	const glm::mat4 inverse = glm::inverse(proj * view);
	int i = 0;
	for (int x = -1; x <= 1; x+=2)
	{
		for (int y = -1; y <= 1; y+=2)
		{
			for (int z = -1; z <= 1; z+=2)
			{
				const glm::vec4 res = inverse * glm::vec4{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f };
				corners[i++] = glm::vec3{ res / res.w };
			}
		}
	}
}

static glm::vec3 GetFrustumMidpoint(const std::array<glm::vec3, 8>& corners)
{
	glm::vec3 sum{ 0.0f };
	for (const glm::vec3& vec : corners)
	{
		sum += vec;
	}
	return sum / 8.0f;
}

static glm::mat4 GetLightProjection(const std::array<glm::vec3, 8>& corners, const glm::mat4& view)
{
	//const glm::vec3 right = glm::vec3{ view[0] };
	//const glm::vec3 up = glm::vec3{ view[1] };
	//const glm::vec3 forward = -glm::vec3{ view[2] };

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (const glm::vec3& vec : corners)
	{
		const glm::vec4 viewVec = view * glm::vec4{ vec, 1.0f };
		minX = std::min(minX, viewVec.x);
		maxX = std::max(maxX, viewVec.x);
		minY = std::min(minY, viewVec.y);
		maxY = std::max(maxY, viewVec.y);
		minZ = std::min(minZ, viewVec.z);
		maxZ = std::max(maxZ, viewVec.z);
	}
	const float worldUnitsPerTexel = (maxX - minX) / 4096.0f;
	minX = std::floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
	minY = std::floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;
	maxX = std::floor(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
	maxY = std::floor(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

	minZ < 0 ? minZ *= 10.0f : minZ /= 10.0f;
	maxZ < 0 ? maxZ /= 10.0f : maxZ *= 10.0f;
	return glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
}

Renderer::Renderer() :
	m_FooShader{ASSETS_PATH "Shaders/Foo.vert", ASSETS_PATH "Shaders/Foo.frag"}
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

	uint32_t lightDepthMaps;
	glGenTextures(1, &lightDepthMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, 4096, 4096, 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	uint32_t depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to complete framebuffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_DepthMapFBO = depthMapFBO;
	m_LightDepthMaps = lightDepthMaps;

	m_FooVBO.SetData(k_CrosshairVertices.data(), k_CrosshairVertices.size());
	m_FooVAO.SetVertexBuffer(m_FooVBO, BufferLayout{ {LayoutElementType::Float, 2}, {LayoutElementType::Float, 2} });
}

void Renderer::Render(const World& world, const Camera& camera) const
{
	static const glm::vec3 lightDir = glm::normalize(glm::vec3{ -1.0f, -1.0f, -1.0f });

	const glm::mat4& view = camera.GetViewMatrix();

	for (int i = 0; i < 4; i++)
	{
		const glm::mat4& proj = camera.GetSubfrustaProjectionMatrix(i);
		std::array<glm::vec3, 8> corners;
		GetFrustumCornersWorldSpace(proj, view, corners);
		const glm::vec3 frustumMidpoint = GetFrustumMidpoint(corners);
		const glm::mat4 lightView = glm::lookAt(frustumMidpoint - lightDir, frustumMidpoint, glm::vec3{ 0.0f, 1.0f, 0.0f });
		const glm::mat4 lightProj = GetLightProjection(corners, lightView);
		const glm::mat4 lightProjView = lightProj * lightView;
		m_MatrixUBO.SetData((i + 2) * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightProjView));
	}

	m_MatrixUBO.SetData(0u, sizeof(glm::mat4), glm::value_ptr(camera.GetProjectionMatrix()));
	m_MatrixUBO.SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));

	glViewport(0, 0, 4096, 4096);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	m_ChunkRenderer.RenderDepth(world.GetOpaqueChunkRenderList(), world.GetTransparentChunkRenderList());

	glViewport(0, 0, 2560, 1440);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	
	m_ChunkRenderer.Render(world.GetOpaqueChunkRenderList(), world.GetTransparentChunkRenderList(), m_LightDepthMaps, camera);
	m_BlockOutlineRenderer.Render(world, camera);
	m_CrosshairRenderer.Render();
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}