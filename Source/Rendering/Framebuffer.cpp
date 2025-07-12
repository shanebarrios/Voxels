#include "Framebuffer.h"
#include <glad/glad.h>
#include "Utils/Logger.h"

static bool IsDepthAttachment(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::Depth32F:
		return true;
	default:
		return false;
	}
}

static GLenum TextureTypeToTarget(FramebufferTextureType type)
{
	switch (type)
	{
	case FramebufferTextureType::Texture2D:
		return GL_TEXTURE_2D;
	case FramebufferTextureType::Texture2DArray:
		return GL_TEXTURE_2D_ARRAY;
	}
}

static GLenum TextureFormatToInternalGL(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::RGBA8:
		return GL_RGBA8;
	case FramebufferTextureFormat::RGBA16F:
		return GL_RGBA16F;
	case FramebufferTextureFormat::Depth32F:
		return GL_DEPTH_COMPONENT32F;
	}
}

static GLenum TextureFormatToGL (FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::RGBA8:
	case FramebufferTextureFormat::RGBA16F:
		return GL_RGBA;
	case FramebufferTextureFormat::Depth32F:
		return GL_DEPTH_COMPONENT;
	}
}

static GLenum TextureFormatToType(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::RGBA8:
		return GL_UNSIGNED_BYTE;
	case FramebufferTextureFormat::RGBA16F:
		return GL_FLOAT;
	case FramebufferTextureFormat::Depth32F:
		return GL_FLOAT;
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

	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);

	m_DepthAttachment = 0;
	m_ColorAttachments.clear();

	for (const FramebufferAttachment& attachment : attachments)
	{
		const GLenum target = TextureTypeToTarget(attachment.Type);
		const GLenum internalFormat = TextureFormatToInternalGL(attachment.Format);
		const GLenum format = TextureFormatToGL(attachment.Format);
		const GLenum type = TextureFormatToType(attachment.Format);
		const GLenum attachmentType = IsDepthAttachment(attachment.Format) ?
			GL_DEPTH_ATTACHMENT :
			GL_COLOR_ATTACHMENT0 + m_ColorAttachments.size();

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

		if (IsDepthAttachment(attachment.Format))
		{
			m_DepthAttachment = id;
		}
		else
		{
			m_ColorAttachments.push_back(id);
		}
	}

	glDrawBuffers(m_ColorAttachments.size(), k_ColorAttachmentEnums.data());

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
	m_Renderbuffer{ other.m_Renderbuffer },
	m_ColorAttachments{ std::move(other.m_ColorAttachments) }
{
	other.m_ID = 0;
	other.m_Renderbuffer = 0;
	other.m_DepthAttachment = 0;
	other.m_ColorAttachments.clear();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (&other == this) return *this;

	Destroy();

	m_ID = other.m_ID;
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_Renderbuffer = other.m_Renderbuffer;
	m_DepthAttachment = other.m_DepthAttachment;
	m_ColorAttachments = std::move(other.m_ColorAttachments);

	other.m_ID = 0;
	other.m_Renderbuffer = 0;
	other.m_DepthAttachment = 0;
	other.m_ColorAttachments.clear();

	return *this;
}

void Framebuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_ID);
	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);
	glDeleteRenderbuffers(1, &m_Renderbuffer);
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