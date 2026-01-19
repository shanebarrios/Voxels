#pragma once

#include <vector>
#include <cstdint>
#include <glad/glad.h>

class VertexBuffer
{
  public:
    VertexBuffer();

    template <typename T>
    explicit VertexBuffer(const std::vector<T>& vertices);

    template <typename T>
    VertexBuffer(const T* vertices, size_t count);

    ~VertexBuffer();
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) noexcept;
    VertexBuffer& operator=(VertexBuffer&&) noexcept;

    template <typename T>
    void SetData(const std::vector<T>& vertices) const;

    template <typename T>
    void SetData(const T* vertices, size_t count) const;

    void Bind() const;

  private:
    uint32_t m_ID = 0;
};

inline VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &m_ID);
}

template <typename T>
inline VertexBuffer::VertexBuffer(const std::vector<T>& vertices)
    : VertexBuffer{}
{
    SetData<T>(vertices);
}

template <typename T>
inline VertexBuffer::VertexBuffer(const T* vertices, size_t count)
    : VertexBuffer{}
{
    SetData<T>(vertices, count);
}

template <typename T>
inline void VertexBuffer::SetData(const std::vector<T>& vertices) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template <typename T>
inline void VertexBuffer::SetData(const T* vertices, size_t count) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

class IndexBuffer
{
  public:
    IndexBuffer();

    explicit IndexBuffer(const std::vector<uint32_t>& indices);

    IndexBuffer(const uint32_t* indices, size_t count);

    ~IndexBuffer();
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) noexcept;
    IndexBuffer& operator=(IndexBuffer&&) noexcept;

    void SetData(const std::vector<uint32_t>& indices) const;

    void SetData(const uint32_t* indices, size_t count) const;

    void Bind() const;

    size_t Count() const { return m_Count; }

  private:
    uint32_t m_ID = 0;
    size_t m_Count = 0;
};

class UniformBuffer
{
  public:
    explicit UniformBuffer(size_t size);

    ~UniformBuffer();
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) noexcept;
    UniformBuffer& operator=(UniformBuffer&&) noexcept;

    void SetData(size_t offset, size_t size, const void* data) const;

    uint32_t GetBindingPoint() const { return m_BindingPoint; }

  private:
    static uint32_t s_LastBindingPoint;
    uint32_t m_BindingPoint = 0;
    uint32_t m_ID = 0;
};
