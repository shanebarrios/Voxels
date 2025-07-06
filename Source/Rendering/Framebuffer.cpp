#include "Framebuffer.h"
#include <glad/glad.h>

Framebuffer::Framebuffer(int width, int height, const std::vector<Attachment>& attachments) :
	m_Width {width},
	m_Height {height}
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(1, m_ID);

}