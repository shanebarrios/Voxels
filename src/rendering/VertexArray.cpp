#include "VertexArray.h"
#include <cassert>

static size_t GetSizeOfType(LayoutElementType type)
{
	switch (type)
	{
	case LayoutElementType::Byte:
	case LayoutElementType::UByte:
		return 1;
	case LayoutElementType::Short:
	case LayoutElementType::UShort:
		return 2;
	case LayoutElementType::Int:
	case LayoutElementType::UInt:
	case LayoutElementType::Float:
		return 4;
	default:
        assert(false && "Unsupported layout element type");
		return 0;
	}
}

BufferLayout::BufferLayout(std::initializer_list<BufferLayoutElement> elements) 
    : m_Elements{ elements } 
{
    for (const BufferLayoutElement& element : elements)
    {
        m_Stride += element.Count * GetSizeOfType(element.Type);
    }
}

void BufferLayout::Add(LayoutElementType type, size_t count)
{
	m_Elements.emplace_back(type, count);
	m_Stride += count * GetSizeOfType(type);
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
	if (m_ID != 0)
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}

VertexArray::VertexArray(VertexArray&& other) noexcept :
	m_ID{ other.m_ID }
{
	other.m_ID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (&other == this)
    {
        return *this;
    }
    if (m_ID != 0)
    {
        glDeleteVertexArrays(1, &m_ID);
    }
    m_ID = other.m_ID;
    other.m_ID = 0;

    return *this;
}

void VertexArray::SetVertexBuffer(const VertexBuffer& VBO, const BufferLayout& layout) const
{
    Bind();
    VBO.Bind();

    const std::vector<BufferLayoutElement>& elements = layout.GetElements();
    const size_t stride = layout.GetStride();
    size_t offset = 0;

    for (size_t i = 0; i < elements.size(); i++)
    {
        const BufferLayoutElement& element = elements[i];
        if (element.Type == LayoutElementType::Float)
        {
            glVertexAttribPointer
            (
                static_cast<GLuint>(i), 
                static_cast<GLint>(element.Count), 
                static_cast<GLenum>(element.Type), 
                GL_FALSE, static_cast<GLsizei>(stride), 
                reinterpret_cast<const GLvoid*>(offset)
            );
        }
        else
        {
            glVertexAttribIPointer
            (
                static_cast<GLuint>(i), 
                static_cast<GLint>(element.Count), 
                static_cast<GLenum>(element.Type), 
                static_cast<GLsizei>(stride), 
                reinterpret_cast<const GLvoid*>(offset)
            );
        }
        glEnableVertexAttribArray(i);
        offset += GetSizeOfType(element.Type) * element.Count;
    }
}

void VertexArray::SetIndexBuffer(const IndexBuffer& ebo) const
{
    Bind();
    ebo.Bind();
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}