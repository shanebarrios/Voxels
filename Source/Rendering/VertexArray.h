#pragma once

#include <cstdint>
#include <glad/glad.h>
#include <vector>
#include "Buffer.h"

// Heavily inspired by the Cherno

enum class LayoutElementType
{
    Byte = GL_BYTE,
    UByte = GL_UNSIGNED_BYTE,
    Short = GL_SHORT,
    UShort = GL_UNSIGNED_SHORT,
    Int = GL_INT,
    UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT,
};

struct BufferLayoutElement
{
    LayoutElementType Type;
    size_t Count;
};

class BufferLayout
{
  public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferLayoutElement> elements);

    void Add(LayoutElementType type, size_t count);

    const std::vector<BufferLayoutElement>& GetElements() const
    {
        return m_Elements;
    }
    size_t GetStride() const { return m_Stride; }

  private:
    std::vector<BufferLayoutElement> m_Elements{};
    size_t m_Stride = 0;
};

class VertexArray
{
  public:
    VertexArray();

    ~VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&) noexcept;
    VertexArray& operator=(VertexArray&&) noexcept;

    // Currently only supports one vertex buffer
    void SetVertexBuffer(const VertexBuffer& VBO,
                         const BufferLayout& layout) const;

    void SetIndexBuffer(const IndexBuffer& EBO) const;

    void Bind() const;

    void Unbind() const;

  private:
    uint32_t m_ID = 0;
};