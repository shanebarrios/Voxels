#pragma once
#include <cstdint>
#include <vector>
#include <optional>
#include <glad/glad.h>
#include "Texture.h"

class Framebuffer
{
public:
	enum class AttachmentType
	{
		Color = GL_RGB,
		Depth,
		Format
	};

	Framebuffer(int width, int height);
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) noexcept;
	Framebuffer& operator=(Framebuffer&&) noexcept;

	void AddAttachment(Texture2D::Format attachmentFormat);

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