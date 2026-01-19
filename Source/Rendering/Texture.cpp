#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <cassert>
#include "Core/Logger.h"

static GLenum TextureInternalFormatToInternalGL(TextureInternalFormat format)
{
    switch (format)
    {
    case TextureInternalFormat::R8: return GL_R8;
    case TextureInternalFormat::RG8: return GL_RG8;
    case TextureInternalFormat::RGB8: return GL_RGB8;
    case TextureInternalFormat::RGBA8: return GL_RGBA8;
    case TextureInternalFormat::RGBA16F: return GL_RGBA16F;
    case TextureInternalFormat::RGBA32F: return GL_RGBA32F;
    case TextureInternalFormat::Depth32F: return GL_DEPTH_COMPONENT32F;
    case TextureInternalFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
    case TextureInternalFormat::Depth24: return GL_DEPTH_COMPONENT24;
    default: assert(false && "Invalid format passed in"); return GL_ZERO;
    }
}

static TextureInternalFormat TextureInternalFormatFromChannels(int numChannels)
{
    switch (numChannels)
    {
    case 1: return TextureInternalFormat::R8;
    case 2: return TextureInternalFormat::RG8;
    case 3: return TextureInternalFormat::RGB8;
    case 4: return TextureInternalFormat::RGBA8;
    default:
        assert(false && "Invalid number of texture channels");
        return TextureInternalFormat::None;
    }
}

static GLenum TextureInternalFormatToGL(TextureInternalFormat format)
{
    switch (format)
    {
    case TextureInternalFormat::R8: return GL_RED;
    case TextureInternalFormat::RG8: return GL_RG;
    case TextureInternalFormat::RGB8: return GL_RGB;
    case TextureInternalFormat::RGBA8:
    case TextureInternalFormat::RGBA16F:
    case TextureInternalFormat::RGBA32F: return GL_RGBA;
    case TextureInternalFormat::Depth32F:
    case TextureInternalFormat::Depth24: return GL_DEPTH_COMPONENT;
    case TextureInternalFormat::Depth24Stencil8: return GL_DEPTH_STENCIL;
    default: assert(false && "Invalid format passed in"); return GL_NONE;
    }
}

static GLenum TextureInternalFormatToStorageType(TextureInternalFormat format)
{
    switch (format)
    {
    case TextureInternalFormat::R8:
    case TextureInternalFormat::RG8:
    case TextureInternalFormat::RGB8:
    case TextureInternalFormat::RGBA8: return GL_UNSIGNED_BYTE;
    case TextureInternalFormat::RGBA16F:
    case TextureInternalFormat::RGBA32F:
    case TextureInternalFormat::Depth32F: return GL_FLOAT;
    case TextureInternalFormat::Depth24: return GL_UNSIGNED_INT;
    case TextureInternalFormat::Depth24Stencil8: return GL_UNSIGNED_INT_24_8;
    default: assert(false && "Invalid format passed in"); return GL_NONE;
    }
}

static uint32_t CreateAndBindTexture(TextureWrap wrapMethod)
{
    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    static_cast<GLenum>(wrapMethod));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    static_cast<GLenum>(wrapMethod));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return id;
}

Texture2D Texture2D::FromPath(std::string_view path, TextureWrap wrapMethod)
{
    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data =
        stbi_load(path.data(), &width, &height, &numChannels, 0);

    if (!data)
    {
        LOG_ERROR("Failed to load texture at {}", path);
        return Texture2D{0, TextureInternalFormat::None, 0, 0};
    }

    const TextureInternalFormat format =
        TextureInternalFormatFromChannels(numChannels);
    Texture2D ret =
        FromData(data, width, height, format, TextureWrap::ClampToEdge);
    stbi_image_free(data);
    return ret;
}

Texture2D Texture2D::FromData(const void* data, int width, int height,
                              TextureInternalFormat format,
                              TextureWrap wrapMethod)
{
    const uint32_t id = CreateAndBindTexture(wrapMethod);
    const GLenum GLFormat = TextureInternalFormatToGL(format);
    const GLenum internalGLFormat = TextureInternalFormatToInternalGL(format);
    const GLenum storageType = TextureInternalFormatToStorageType(format);

    glTexImage2D(GL_TEXTURE_2D, 0, internalGLFormat, width, height, 0, GLFormat,
                 storageType, data);
    return Texture2D{id, format, width, height};
}

Texture2D::Texture2D(uint32_t id, TextureInternalFormat format, int width,
                     int height)
    : m_ID{id}, m_Format{format}, m_Width{width}, m_Height{height}
{
}

Texture2D::~Texture2D()
{
    if (m_ID != 0)
    {
        glDeleteTextures(1, &m_ID);
    }
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : m_ID{other.m_ID}, m_Format{other.m_Format}, m_Width{other.m_Width},
      m_Height{other.m_Height}
{
    other.m_ID = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (&other == this)
    {
        return *this;
    }
    if (m_ID != 0)
    {
        glDeleteTextures(1, &m_ID);
    }
    m_ID = other.m_ID;
    m_Format = other.m_Format;
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    other.m_ID = 0;
    return *this;
}

void Texture2D::Bind(int unit) const
{
    assert(unit >= 0 && unit <= 15 && "Invalid texture unit");
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

// std::shared_ptr<TextureCube> TextureCube::Create(const
// std::array<std::string_view, 6>& facePaths)
//{
//     return std::make_shared<TextureCube>(facePaths);
// }
//
// TextureCube::TextureCube(const std::array<std::string_view, 6>& facePaths)
//{
//     glGenTextures(1, &m_ID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
//     GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_CUBE_MAP,
//     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
//     GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_CUBE_MAP,
//     GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_CUBE_MAP,
//     GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//     stbi_set_flip_vertically_on_load(false);
//
//     int width, height, numChannels;
//     for (size_t i = 0; i < facePaths.size(); i++)
//     {
//         unsigned char* data = stbi_load(facePaths[i].data(), &width, &height,
//         &numChannels, 0); if (!data)
//         {
//             LOG_ERROR("Failed to load texture at {}", facePaths[i]);
//         }
//
//         const GLenum format = FormatFromChannels(numChannels);
//         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width,
//         height, 0, format, GL_UNSIGNED_BYTE, data); stbi_image_free(data);
//     }
// }
//
// TextureCube::~TextureCube()
//{
//     if (m_ID != 0)
//     {
//         glDeleteTextures(1, &m_ID);
//     }
// }
//
// TextureCube::TextureCube(TextureCube&& other) noexcept : m_ID{ other.m_ID }
//{
//     other.m_ID = 0;
// }
//
// TextureCube& TextureCube::operator=(TextureCube&& other) noexcept
//{
//     if (&other == this)
//     {
//         return *this;
//     }
//     if (m_ID != 0)
//     {
//         glDeleteTextures(1, &m_ID);
//     }
//     m_ID = other.m_ID;
//     other.m_ID = 0;
//     return *this;
// }
//
// void TextureCube::Bind(int unit) const
//{
//     assert(unit >= 0 && unit <= 15 && "Invalid texture unit");
//     glActiveTexture(GL_TEXTURE0 + unit);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
// }
// GLuint TextureCube::GetId() const
//{
//     return m_ID;
// }
