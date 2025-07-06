#include "Framebuffer.h"
#include <glad/glad.h>

Framebuffer::Framebuffer(int width, int height) :
	m_Width {width},
	m_Height {height}
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glDrawBuffer(0);
	glReadBuffer(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_ID);
}

void Framebuffer::AddAttachment(Texture2D::Format attachmentFormat)
{
	Texture2D texture = Texture2D::Attachment(m_Width, m_Height, attachmentFormat);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(attachmentFormat), GL_TEXTURE_2D, texture.GetId(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}