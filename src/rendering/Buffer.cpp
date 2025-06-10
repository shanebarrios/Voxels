#include "Buffer.h"

VertexBuffer::~VertexBuffer()
{
	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept : 
	m_ID{ other.m_ID }
{
	other.m_ID = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}

	m_ID = other.m_ID;
	other.m_ID = 0;
	return *this;
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

IndexBuffer::IndexBuffer()
{
	glGenBuffers(1, &m_ID);
}

IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices) :
	IndexBuffer{}
{
	SetData(indices);
}

IndexBuffer::IndexBuffer(const uint32_t* indices, size_t count) :
	IndexBuffer{}
{
	SetData(indices, count);
}

IndexBuffer::~IndexBuffer()
{
	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept :
	m_ID{ other.m_ID },
	m_Count {other.m_Count }
{
	other.m_ID = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}

	m_ID = other.m_ID;
	m_Count = other.m_Count;
	other.m_ID = 0;
	return *this;
}

void IndexBuffer::SetData(const std::vector<uint32_t>& indices) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetData(const uint32_t* indices, size_t count) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

uint32_t UniformBuffer::s_LastBindingPoint = 0;

UniformBuffer::UniformBuffer(size_t size) :
	m_BindingPoint {++s_LastBindingPoint}
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept :
	m_ID {other.m_ID},
	m_BindingPoint {other.m_BindingPoint}
{
	other.m_ID = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	if (m_ID != 0)
	{
		glDeleteBuffers(1, &m_ID);
	}
	m_ID = other.m_ID;
	m_BindingPoint = other.m_BindingPoint;
	other.m_ID = 0;
	return *this;
}

void UniformBuffer::SetData(size_t offset, size_t size, const void* data) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}