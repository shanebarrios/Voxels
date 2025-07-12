#pragma once

#include <glad/glad.h>
#include <string_view>
#include <memory>
#include <array>

class Texture
{
public:
	virtual ~Texture() = default;
	virtual void Bind(int unit = 0) const = 0;
	virtual uint32_t GetId() const = 0;
};

class Texture2D : public Texture
{
public:
	enum class Format
	{
		R = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		Depth = GL_DEPTH_COMPONENT,
		Stencil = GL_STENCIL_INDEX,
		DepthStencil = GL_DEPTH_STENCIL
	};

	Texture2D() = default;

	static Texture2D FromPath(std::string_view path);

	~Texture2D();

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	Texture2D(Texture2D&&) noexcept;
	Texture2D& operator=(Texture2D&&) noexcept;

	void Bind(int unit = 0) const override;
	GLuint GetId() const override { return m_ID; }

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	Format GetFormat() const { return m_Format; }

private:
	explicit Texture2D(uint32_t id, Format format, int width, int height);

	uint32_t m_ID = 0;
	Format m_Format = Format::R;
	int m_Width = 0;
	int m_Height = 0;
};

//class TextureCube : public Texture
//{
//public:
//	static std::shared_ptr<TextureCube> Create(const std::array<std::string_view, 6>& facePaths);
//
//	TextureCube(const std::array<std::string_view, 6>& facePaths);
//	~TextureCube();
//
//	TextureCube(const TextureCube&) = delete;
//	TextureCube& operator=(const TextureCube&) = delete;
//	TextureCube(TextureCube&&) noexcept;
//	TextureCube& operator=(TextureCube&&) noexcept;
//
//	void Bind(int unit = 0) const override;
//	GLuint GetId() const override;
//private:
//	GLuint m_ID{};
//};