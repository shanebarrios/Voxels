#include "Framebuffer.h"
#include <glad/glad.h>
#include "Utils/Logger.h"
#include "Utils/Common.h"

static bool IsColorAttachment(FramebufferAttachmentFormat format)
{
	switch (format)
	{
	case FramebufferAttachmentFormat::Depth32F:
	case FramebufferAttachmentFormat::Depth24Stencil8:
	case FramebufferAttachmentFormat::Depth24:
		return false;
	default:
		return true;
	}
}
static GLenum AttachmentFormatToAttachment(FramebufferAttachmentFormat format, uint8_t numColorAttachments)
{
	switch (format)
	{
	case FramebufferAttachmentFormat::Depth32F:
	case FramebufferAttachmentFormat::Depth24:
		return GL_DEPTH_ATTACHMENT;
	case FramebufferAttachmentFormat::Depth24Stencil8:
		return GL_DEPTH_STENCIL_ATTACHMENT;
	default:
		return GL_COLOR_ATTACHMENT0 + numColorAttachments;
	}
}

static bool IsTextureAttachment(FramebufferAttachmentType type)
{
	switch (type)
	{
	case FramebufferAttachmentType::Renderbuffer:
		return false;
	default:
		return true;
	}
}

static GLenum AttachmentTypeToTarget(FramebufferAttachmentType type)
{
	switch (type)
	{
	case FramebufferAttachmentType::Texture2D:
		return GL_TEXTURE_2D;
	case FramebufferAttachmentType::Texture2DArray:
		return GL_TEXTURE_2D_ARRAY;
	case FramebufferAttachmentType::Renderbuffer:
		return GL_RENDERBUFFER;
	default:
		unreachable();
	}
}

static GLenum AttachmentFormatToInternalGL(FramebufferAttachmentFormat format)
{
	switch (format)
	{
	case FramebufferAttachmentFormat::R32F:
		return GL_R32F;
	case FramebufferAttachmentFormat::R8:
		return GL_R8;
	case FramebufferAttachmentFormat::RG8:
		return GL_RG8;
	case FramebufferAttachmentFormat::RGB8:
		return GL_RGB8;
	case FramebufferAttachmentFormat::RGBA8:
		return GL_RGBA8;
	case FramebufferAttachmentFormat::RGBA16F:
		return GL_RGBA16F;
	case FramebufferAttachmentFormat::RGBA32F:
		return GL_RGBA32F;
	case FramebufferAttachmentFormat::Depth32F:
		return GL_DEPTH_COMPONENT32F;
	case FramebufferAttachmentFormat::Depth24Stencil8:
		return GL_DEPTH24_STENCIL8;
	case FramebufferAttachmentFormat::Depth24:
		return GL_DEPTH_COMPONENT24;
	default:
		unreachable();
	}
}

static GLenum AttachmentFormatToGL (FramebufferAttachmentFormat format)
{
	switch (format)
	{
	case FramebufferAttachmentFormat::R32F:
	case FramebufferAttachmentFormat::R8:
		return GL_RED;
	case FramebufferAttachmentFormat::RG8:
		return GL_RG;
	case FramebufferAttachmentFormat::RGB8:
		return GL_RGB;
	case FramebufferAttachmentFormat::RGBA8:
	case FramebufferAttachmentFormat::RGBA16F:
	case FramebufferAttachmentFormat::RGBA32F:
		return GL_RGBA;
	case FramebufferAttachmentFormat::Depth32F:
	case FramebufferAttachmentFormat::Depth24:
		return GL_DEPTH_COMPONENT;
	case FramebufferAttachmentFormat::Depth24Stencil8:
		return GL_DEPTH_STENCIL;
	default:
		unreachable();
	}
}

static GLenum AttachmentFormatToType(FramebufferAttachmentFormat format)
{
	switch (format)
	{
	case FramebufferAttachmentFormat::R8:
	case FramebufferAttachmentFormat::RG8:
	case FramebufferAttachmentFormat::RGB8:
	case FramebufferAttachmentFormat::RGBA8:
		return GL_UNSIGNED_BYTE;
	case FramebufferAttachmentFormat::R32F:
	case FramebufferAttachmentFormat::RGBA16F:
	case FramebufferAttachmentFormat::RGBA32F:
	case FramebufferAttachmentFormat::Depth32F:
		return GL_FLOAT;
	case FramebufferAttachmentFormat::Depth24Stencil8:
		return GL_UNSIGNED_INT_24_8;
	case FramebufferAttachmentFormat::Depth24:
		return GL_UNSIGNED_INT;
	default:
		unreachable();
	}
}

static constexpr std::array<GLenum, 32> ColorAttachmentEnums()
{
	std::array<GLenum, 32> ret;
	for (size_t i = 0; i < 32; i++)
	{
		ret[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	return ret;
}

static constexpr std::array<GLenum, 32> k_ColorAttachmentEnums = ColorAttachmentEnums();

Framebuffer::Framebuffer(int width, int height) :
	m_Width{ width },
	m_Height{ height }
{
	glGenFramebuffers(1, &m_ID);
}

Framebuffer::Framebuffer(int width, int height, const std::vector<FramebufferAttachment>& attachments) :
	m_Width {width},
	m_Height {height}
{
	glGenFramebuffers(1, &m_ID);

	SetAttachments(attachments);
}

void Framebuffer::SetAttachments(const std::vector<FramebufferAttachment>& attachments)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	glDeleteTextures(m_TextureAttachments.size(), m_TextureAttachments.data());
	glDeleteRenderbuffers(m_RenderbufferAttachments.size(), m_RenderbufferAttachments.data());

	m_TextureAttachments.clear();
	m_RenderbufferAttachments.clear();
	m_NumColorAttachments = 0;

	for (const FramebufferAttachment& attachment : attachments)
	{
		if (IsTextureAttachment(attachment.Type))
		{
			AddTextureAttachment(attachment);
		}
		else
		{
			AddRenderbufferAttachment(attachment);
		}
	}

	glDrawBuffers(m_NumColorAttachments, k_ColorAttachmentEnums.data());

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to complete framebuffer!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	Destroy();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept :
	m_ID{ other.m_ID },
	m_Width{ other.m_Width },
	m_Height{ other.m_Height },
	m_NumColorAttachments{ other.m_NumColorAttachments },
	m_TextureAttachments{ std::move(other.m_TextureAttachments) },
	m_RenderbufferAttachments{ std::move(other.m_RenderbufferAttachments) }
{
	other.m_ID = 0;
	other.m_TextureAttachments.clear();
	other.m_RenderbufferAttachments.clear();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (&other == this) return *this;

	Destroy();

	m_ID = other.m_ID;
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_TextureAttachments = std::move(other.m_TextureAttachments);
	m_RenderbufferAttachments = std::move(other.m_RenderbufferAttachments);

	other.m_ID = 0;
	other.m_TextureAttachments.clear();
	other.m_RenderbufferAttachments.clear();

	return *this;
}

void Framebuffer::AddTextureAttachment(const FramebufferAttachment& attachment)
{
	const GLenum target = AttachmentTypeToTarget(attachment.Type);
	const GLenum internalFormat = AttachmentFormatToInternalGL(attachment.Format);
	const GLenum format = AttachmentFormatToGL(attachment.Format);
	const GLenum type = AttachmentFormatToType(attachment.Format);
	const GLenum attachmentType = AttachmentFormatToAttachment(attachment.Format, m_NumColorAttachments);

	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(target, id);
	if (target == GL_TEXTURE_2D)
	{
		glTexImage2D(target, 0, internalFormat, m_Width, m_Height, 0, format, type, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target, id, 0);
	}
	else if (target == GL_TEXTURE_2D_ARRAY)
	{
		glTexImage3D(target, 0, internalFormat, m_Width, m_Height, attachment.LayerCount, 0, format, type, nullptr);
		glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, id, 0);
	}
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	m_TextureAttachments.push_back(id);

	if (IsColorAttachment(attachment.Format))
	{
		m_NumColorAttachments++;
	}
}

void Framebuffer::AddRenderbufferAttachment(const FramebufferAttachment& attachment)
{
	const GLenum internalFormat = AttachmentFormatToInternalGL(attachment.Format);
	const GLenum attachmentType = AttachmentFormatToAttachment(attachment.Format, m_NumColorAttachments);

	uint32_t id;
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, id);
	m_RenderbufferAttachments.push_back(id);

	if (IsColorAttachment(attachment.Format))
	{
		m_NumColorAttachments++;
	}
}

void Framebuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_ID);
	glDeleteTextures(m_TextureAttachments.size(), m_TextureAttachments.data());
	glDeleteRenderbuffers(m_RenderbufferAttachments.size(), m_RenderbufferAttachments.data());
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glViewport(0, 0, m_Width, m_Height);
}

void Framebuffer::Unbind(int defaultWidth, int defaultHeight) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, defaultWidth, defaultHeight);
}