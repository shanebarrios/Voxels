#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string_view>
#include <string_view>
#include <cstdint>
#include <array>

class UniformBuffer;

class Shader
{
public:
    enum ShaderUniform : uint8_t
    {
        UNIFORM_POSITION,
        UNIFORM_TEXTURE_ATLAS,
        UNIFORM_SHADOW_MAP,
        UNIFORM_SUBFRUSTA_PLANES,
        UNIFORM_POSITION_SAMPLER,
        UNIFORM_NORMAL_SAMPLER,
        UNIFORM_ALBEDO_SAMPLER,
        UNIFORM_NOISE_SAMPLER,
        UNIFORM_SAMPLES,
        UNIFORM_TRANSFORM,
        UNIFORM_LIGHT_DIR,
        UNIFORM_COUNT
    };

    Shader(std::string_view vertPath, std::string_view fragPath);
    Shader(std::string_view vertPath, std::string_view fragPath, std::string_view geomPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;

    uint32_t GetId() const { return m_ID; };
    void Bind() const;

    void BindUniformBlock(uint32_t bindingPoint, std::string_view name) const;

    void SetUniform(ShaderUniform uniform, bool val) const
    {
        glUniform1i(m_UniformLocations[uniform], val);
    }

    void SetUniform(ShaderUniform uniform, int val) const
    {
        glUniform1i(m_UniformLocations[uniform], val);
    }

    void SetUniform(ShaderUniform uniform, float val) const
    {
        glUniform1f(m_UniformLocations[uniform], val);
    }

    void SetUniform(ShaderUniform uniform, const glm::vec2& val) const
    {
        glUniform2f(m_UniformLocations[uniform], val.x, val.y);
    }

    void SetUniform(ShaderUniform uniform, const glm::vec3& val) const
    {
        glUniform3f(m_UniformLocations[uniform], val.x, val.y, val.z);
    }

    void SetUniform(ShaderUniform uniform, const glm::vec4& val) const
    {
        glUniform4f(m_UniformLocations[uniform], val.x, val.y, val.z, val.w);
    }

    void SetUniform(ShaderUniform uniform, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(m_UniformLocations[uniform], 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetUniform(ShaderUniform uniform, const glm::ivec2& val) const
    {
        glUniform2i(m_UniformLocations[uniform], val.x, val.y);
    }

    void SetUniform(ShaderUniform uniform, const glm::ivec3& val) const
    {
        glUniform3i(m_UniformLocations[uniform], val.x, val.y, val.z);
    }

    void SetUniform(ShaderUniform uniform, int x, int y, int z) const
    {
        glUniform3i(m_UniformLocations[uniform], x, y, z);
    }

    void SetUniform(ShaderUniform uniform, int x, int y, int z, int w) const
    {
        glUniform4i(m_UniformLocations[uniform], x, y, z, w);
    }

    void SetUniform(ShaderUniform uniform, float x, float y, float z, float w) const
    {
        glUniform4f(m_UniformLocations[uniform], x, y, z, w);
    }

    void SetUniform(ShaderUniform uniform, const float* vs, size_t count) const
    {
        glUniform1fv(m_UniformLocations[uniform], count, vs);
    }

    void SetUniform(ShaderUniform uniform, const glm::vec3* vs, size_t count) const
    {
        glUniform3fv(m_UniformLocations[uniform], count, glm::value_ptr(vs[0]));
    }

private:
    uint32_t m_ID{};

    std::array<int, UNIFORM_COUNT> m_UniformLocations{};

    int GetUniformLoc(std::string_view name) const;
    void CacheUniformLocations();
};



