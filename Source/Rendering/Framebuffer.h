#pragma once
#include <cstdint>
#include <vector>
#include <optional>

class Texture2D;

struct FramebufferAttachment
{
	enum AttachmentType
	{
		Color,
		Depth,
		Stencil,
		DepthStencil
	};
};

class Framebuffer
{
public:
	enum class Attachment
	{
		Color,
		Depth,
		Stencil,
		DepthStencil
	};

	Framebuffer(int width, int height, const std::vector<Attachment>& attachments);
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) noexcept;
	Framebuffer& operator=(Framebuffer&&) noexcept;

	Texture2D GetColorAttachment(int index) const;

	Texture2D GetDepthAttachment() const;

	Texture2D GetStencilAttachment() const;

	Texture2D GetDepthStencilAttachment() const;

	void Bind() const;
	
private:
	uint32_t m_ID = 0;
	int m_Width = 0;
	int m_Height = 0;

	std::vector<Texture2D> m_ColorAttachments;
	std::optional<Texture2D> m_DepthAttachment;
	std::optional<Texture2D> m_StencilAttachment;
	std::optional<Texture2D> m_DepthStencilAttachment;
};