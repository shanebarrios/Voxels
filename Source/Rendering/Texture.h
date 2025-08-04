#pragma once

#include <glad/glad.h>
#include <string_view>
#include <memory>
#include <array>

enum class TextureWrap
{
	Repeat = GL_REPEAT,
	MirroredRepeat = GL_MIRRORED_REPEAT,
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER
};

enum class TextureInternalFormat
{
	None,
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
	Texture2D() = default;

	static Texture2D FromPath(std::string_view path, 
		TextureWrap wrapMethod = TextureWrap::ClampToEdge);

	static Texture2D FromData(const void* data, int width, int height, 
		TextureInternalFormat format, TextureWrap wrapMethod = TextureWrap::ClampToEdge);

	~Texture2D();

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	Texture2D(Texture2D&&) noexcept;
	Texture2D& operator=(Texture2D&&) noexcept;

	void Bind(int unit = 0) const override;
	GLuint GetId() const override { return m_ID; }

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	TextureInternalFormat GetInternalFormat() const { return m_Format; }

private:
	explicit Texture2D(uint32_t id, TextureInternalFormat format, int width, int height);

private:
	uint32_t m_ID = 0;
	TextureInternalFormat m_Format = TextureInternalFormat::R8;
	int m_Width = 0;
	int m_Height = 0;
};

class Texture2DArray : public Texture
{
	Texture2DArray() = default;
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