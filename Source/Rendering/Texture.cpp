#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <cassert>
#include "Utils/Logger.h"

static Texture2D::Format FormatFromChannels(int numChannels)
{
    switch (numChannels)
    {
    case 1: return Texture2D::Format::R;
    case 2: return Texture2D::Format::RG;
    case 3: return Texture2D::Format::RGB;
    case 4: return Texture2D::Format::RGBA;
    default: assert(false && "Invalid number of texture channels"); return Texture2D::Format::R;
    }
}

static GLenum FormatToInternal(Texture2D::Format format)
{
    switch (format)
    {
    case Texture2D::Format::R:
        return GL_R8;
    case Texture2D::Format::RG:
        return GL_RG8;
    case Texture2D::Format::RGB:
        return GL_RGB8;
    case Texture2D::Format::RGBA:
        return GL_RGBA8;
    case Texture2D::Format::Depth:
        return GL_DEPTH_COMPONENT24;
    case Texture2D::Format::Stencil:
        return GL_STENCIL_INDEX8;
    case Texture2D::Format::DepthStencil:
        return GL_DEPTH24_STENCIL8;
    default:
        assert(false && "Invalid format passed in"); return GL_R8;
    }
}

static GLenum FormatToType(Texture2D::Format format)
{
    switch (format)
    {
    case Texture2D::Format::R:
    case Texture2D::Format::RG:
    case Texture2D::Format::RGB:
    case Texture2D::Format::RGBA:
    case Texture2D::Format::Stencil:
        return GL_UNSIGNED_BYTE;
    case Texture2D::Format::Depth:
        return GL_FLOAT;
    case Texture2D::Format::DepthStencil:
        return GL_UNSIGNED_INT_24_8;
    default:
        assert(false && "Invalid format passed in"); return GL_UNSIGNED_BYTE;
    }
}

Texture2D Texture2D::FromPath(std::string_view path)
{
    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.data(), &width, &height, &numChannels, 0);

    if (!data)
    {
        LOG_ERROR("Failed to load texture at {}", path);
        return Texture2D{ 0, Format::R, 0, 0 };
    }

    const Format format = FormatFromChannels(numChannels);
    const GLenum internalFormat = FormatToInternal(format);
    const GLenum type = FormatToType(format);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, static_cast<GLenum>(format), type, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return Texture2D{ id, format, width, height };
}

Texture2D::Texture2D(uint32_t id, Format format, int width, int height) :
    m_ID{ id },
    m_Format{ format },
    m_Width{ width },
    m_Height{ height }
{
}

Texture2D::~Texture2D()
{
    if (m_ID != 0)
    {
        glDeleteTextures(1, &m_ID);
    }
}

Texture2D::Texture2D(Texture2D&& other) noexcept : 
    m_ID{ other.m_ID },
    m_Format{ other.m_Format },
    m_Width{ other.m_Width },
    m_Height{  other.m_Height }
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

//std::shared_ptr<TextureCube> TextureCube::Create(const std::array<std::string_view, 6>& facePaths)
//{
//    return std::make_shared<TextureCube>(facePaths);
//}
//
//TextureCube::TextureCube(const std::array<std::string_view, 6>& facePaths)
//{
//    glGenTextures(1, &m_ID);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    stbi_set_flip_vertically_on_load(false);
//
//    int width, height, numChannels;
//    for (size_t i = 0; i < facePaths.size(); i++)
//    {
//        unsigned char* data = stbi_load(facePaths[i].data(), &width, &height, &numChannels, 0);
//        if (!data)
//        {
//            LOG_ERROR("Failed to load texture at {}", facePaths[i]);
//        }
//
//        const GLenum format = FormatFromChannels(numChannels);
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        stbi_image_free(data);
//    }
//}
//
//TextureCube::~TextureCube()
//{
//    if (m_ID != 0)
//    {
//        glDeleteTextures(1, &m_ID);
//    }
//}
//
//TextureCube::TextureCube(TextureCube&& other) noexcept : m_ID{ other.m_ID }
//{
//    other.m_ID = 0;
//}
//
//TextureCube& TextureCube::operator=(TextureCube&& other) noexcept
//{
//    if (&other == this)
//    {
//        return *this;
//    }
//    if (m_ID != 0)
//    {
//        glDeleteTextures(1, &m_ID);
//    }
//    m_ID = other.m_ID;
//    other.m_ID = 0;
//    return *this;
//}
//
//void TextureCube::Bind(int unit) const
//{
//    assert(unit >= 0 && unit <= 15 && "Invalid texture unit");
//    glActiveTexture(GL_TEXTURE0 + unit);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
//}
//GLuint TextureCube::GetId() const
//{
//    return m_ID;
//}
