#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <cassert>
#include "../Logger.h"

static GLenum FormatFromChannels(int numChannels)
{
    switch (numChannels)
    {
    case 1: return GL_RED;
    case 2: return GL_RG;
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    default: assert(false && "Invalid number of texture channels"); return 0;
    }
}

std::shared_ptr<Texture2D> Texture2D::Create(std::string_view path)
{
    return std::shared_ptr<Texture2D>(new Texture2D(path));
}

Texture2D::Texture2D(std::string_view path)
{
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);
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
    }

    const GLenum format = FormatFromChannels(numChannels);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Texture2D::~Texture2D()
{
    if (m_ID != 0)
    {
        glDeleteTextures(1, &m_ID);
    }
}

Texture2D::Texture2D(Texture2D&& other) noexcept : m_ID{ other.m_ID }
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
    other.m_ID = 0;
    return *this;
}

void Texture2D::Bind(int unit) const
{
    assert(unit >= 0 && unit <= 15 && "Invalid texture unit");
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

GLuint Texture2D::GetId() const { return m_ID; }

std::shared_ptr<TextureCube> TextureCube::Create(const std::array<std::string_view, 6>& facePaths)
{
    return std::make_shared<TextureCube>(facePaths);
}

TextureCube::TextureCube(const std::array<std::string_view, 6>& facePaths)
{
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(false);

    int width, height, numChannels;
    for (size_t i = 0; i < facePaths.size(); i++)
    {
        unsigned char* data = stbi_load(facePaths[i].data(), &width, &height, &numChannels, 0);
        if (!data)
        {
            LOG_ERROR("Failed to load texture at {}", facePaths[i]);
        }

        const GLenum format = FormatFromChannels(numChannels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
}

TextureCube::~TextureCube()
{
    if (m_ID != 0)
    {
        glDeleteTextures(1, &m_ID);
    }
}

TextureCube::TextureCube(TextureCube&& other) noexcept : m_ID{ other.m_ID }
{
    other.m_ID = 0;
}

TextureCube& TextureCube::operator=(TextureCube&& other) noexcept
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
    other.m_ID = 0;
    return *this;
}

void TextureCube::Bind(int unit) const
{
    assert(unit >= 0 && unit <= 15 && "Invalid texture unit");
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
}
GLuint TextureCube::GetId() const
{
    return m_ID;
}
