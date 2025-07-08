#include "Shader.h"

#include <fstream>
#include <format>
#include <sstream>
#include <cstring>

#include "Logger.h"

struct ShaderProgramSource
{
    std::string VertSource;
    std::string FragSource;
    std::string GeomSource = "";
};

static std::string ReadFile(std::string_view path)
{
    const std::ifstream inf{ path.data() };
    if (!inf)
    {
        LOG_ERROR("Failed to open shader file {}", path);
        return {};
    }
    std::stringstream ss{};
    ss << inf.rdbuf();
    return ss.str();
}

static uint32_t CompileShader(const std::string& source, GLenum type)
{
    const uint32_t id = glCreateShader(type);

    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int32_t success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        glDeleteShader(id);
        LOG_ERROR("Failed to compile shader: {}", infoLog);
        return 0;
    }
    return id;
}

static uint32_t LinkSources(const ShaderProgramSource& sources)
{
    const uint32_t vertId = CompileShader(sources.VertSource, GL_VERTEX_SHADER);
    const uint32_t fragId = CompileShader(sources.FragSource, GL_FRAGMENT_SHADER);
    const uint32_t geomId = sources.GeomSource.length() > 0 ? CompileShader(sources.GeomSource, GL_GEOMETRY_SHADER) : 0;

    const uint32_t programId = glCreateProgram();
    glAttachShader(programId, vertId);
    glAttachShader(programId, fragId);
    if (geomId)
    {
        glAttachShader(programId, geomId);
    }
    glLinkProgram(programId);

    int32_t success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        glDeleteShader(vertId);
        glDeleteShader(fragId);
        glDeleteProgram(programId);
        LOG_ERROR("Failed to link shader: {}", infoLog);
        return 0;
    }

    glDeleteShader(vertId);
    glDeleteShader(fragId);
    if (geomId)
    {
        glDeleteShader(geomId);
    }
    return programId;
}

Shader::Shader(std::string_view vertPath, std::string_view fragPath)
{
    const ShaderProgramSource sources = { ReadFile(vertPath), ReadFile(fragPath) };
    m_ID = LinkSources(sources);
    CacheUniformLocations();
}

Shader::Shader(std::string_view vertPath, std::string_view fragPath, std::string_view geomPath)
{
    const ShaderProgramSource sources = { ReadFile(vertPath), ReadFile(fragPath), ReadFile(geomPath) };
    m_ID = LinkSources(sources);
    CacheUniformLocations();
}

Shader::~Shader()
{
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
    }
}

Shader::Shader(Shader&& other) noexcept : m_ID{ other.m_ID }
{
    other.m_ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
    }
    m_ID = other.m_ID;
    other.m_ID = 0;
    return *this;
}

void Shader::Bind() const { glUseProgram(m_ID); }

void Shader::BindUniformBlock(uint32_t bindingPoint, std::string_view name) const
{
    const uint32_t blockIndex = glGetUniformBlockIndex(m_ID, name.data());
    if (blockIndex == GL_INVALID_INDEX)
    {
        LOG_WARN("Uniform block {} not found in shader", name);
    }
    glUniformBlockBinding(m_ID, blockIndex, bindingPoint);
}

int Shader::GetUniformLoc(std::string_view name) const
{
    const int ret = glGetUniformLocation(m_ID, name.data());
    if (ret == -1)
    {
        LOG_WARN("Can not find uniform {} in shader", name);
    }
    return ret;
}

void Shader::CacheUniformLocations()
{
    m_UniformLocations[UNIFORM_POSITION] = GetUniformLoc("u_Position");
    m_UniformLocations[UNIFORM_TEXTURE_ATLAS] = GetUniformLoc("u_TextureAtlas");
    m_UniformLocations[UNIFORM_SHADOW_MAP] = GetUniformLoc("u_ShadowMap");
    m_UniformLocations[UNIFORM_SUBFRUSTA_PLANES] = GetUniformLoc("u_SubfrustaPlanes");
}
