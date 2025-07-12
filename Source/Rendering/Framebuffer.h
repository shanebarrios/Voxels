#pragma once
#include <cstdint>
#include <vector>
#include <array>

enum class FramebufferTextureFormat
{
	RGBA8,
	RGBA16F,
	Depth32F
};

enum class FramebufferTextureType
{
	Texture2D,
	Texture2DArray
};

struct FramebufferAttachment
{
	FramebufferTextureFormat Format;
	FramebufferTextureType Type = FramebufferTextureType::Texture2D;
	uint32_t LayerCount = 1;
};

class Framebuffer
{
public:
	Framebuffer(int width, int height);
	Framebuffer(int width, int height, const std::vector<FramebufferAttachment>& attachments);
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) noexcept;
	Framebuffer& operator=(Framebuffer&&) noexcept;

	void SetAttachments(const std::vector<FramebufferAttachment>& attachments);

	uint32_t GetDepthAttachment() const { return m_DepthAttachment; };

	uint32_t GetColorAttachment(size_t index) const { return m_ColorAttachments[index]; }

	void Bind() const;

	void Unbind(int defaultWidth, int defaultHeight) const;
	
private:
	uint32_t m_ID = 0;
	int m_Width = 0;
	int m_Height = 0;

	uint32_t m_Renderbuffer = 0;
	uint32_t m_DepthAttachment = 0;
	std::vector<uint32_t> m_ColorAttachments{};

	void Destroy();
};