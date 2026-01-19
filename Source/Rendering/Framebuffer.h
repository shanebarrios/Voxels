#pragma once
#include <cstdint>
#include <vector>
#include <array>

enum class FramebufferAttachmentFormat
{
    R32F,
    R8,
    RG8,
    RGB8,
    RGBA8,
    RGBA16F,
    RGBA32F,
    Depth32F,
    Depth24Stencil8,
    Depth24
};

enum class FramebufferAttachmentType
{
    Texture2D,
    Texture2DArray,
    Renderbuffer
};

struct FramebufferAttachment
{
    FramebufferAttachmentFormat Format;
    FramebufferAttachmentType Type = FramebufferAttachmentType::Texture2D;
    uint32_t LayerCount = 1;
};

class Framebuffer
{
  public:
    Framebuffer(int width, int height);
    Framebuffer(int width, int height,
                const std::vector<FramebufferAttachment>& attachments);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;

    void SetAttachments(const std::vector<FramebufferAttachment>& attachments);

    uint32_t GetTextureAttachment(size_t index) const
    {
        return m_TextureAttachments[index];
    }

    uint32_t GetId() const { return m_ID; }

    int GetWidth() const { return m_Width; }

    int GetHeight() const { return m_Height; }

    void Bind() const;

    void Unbind(int defaultWidth, int defaultHeight) const;

  private:
    void Destroy();

    void AddTextureAttachment(const FramebufferAttachment& attachment);
    void AddRenderbufferAttachment(const FramebufferAttachment& attachment);

  private:
    uint32_t m_ID = 0;
    int m_Width = 0;
    int m_Height = 0;

    uint8_t m_NumColorAttachments = 0;

    std::vector<uint32_t> m_TextureAttachments{};
    std::vector<uint32_t> m_RenderbufferAttachments{};
};