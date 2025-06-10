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
	virtual GLuint GetId() const = 0;
};

class Texture2D : public Texture
{
public:
	static std::shared_ptr<Texture2D> Create(std::string_view path);
	explicit Texture2D(std::string_view path);

	~Texture2D();

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	Texture2D(Texture2D&&) noexcept;
	Texture2D& operator=(Texture2D&&) noexcept;

	void Bind(int unit = 0) const override;
	GLuint GetId() const override;

private:
	GLuint m_ID{};
};

class TextureCube : public Texture
{
public:
	static std::shared_ptr<TextureCube> Create(const std::array<std::string_view, 6>& facePaths);

	TextureCube(const std::array<std::string_view, 6>& facePaths);
	~TextureCube();

	TextureCube(const TextureCube&) = delete;
	TextureCube& operator=(const TextureCube&) = delete;
	TextureCube(TextureCube&&) noexcept;
	TextureCube& operator=(TextureCube&&) noexcept;

	void Bind(int unit = 0) const override;
	GLuint GetId() const override;
private:
	GLuint m_ID{};
};