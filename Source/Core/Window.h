#pragma once

#include <string_view>

struct GLFWwindow;

class Window
{
public:
	Window(int width, int height, std::string_view name, bool vsync, bool fullscreen);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) noexcept;
	Window& operator=(Window&&) noexcept;

	void SwapBuffers() const;

	void Activate();

	bool ShouldClose() const;

	GLFWwindow* GetHandle() const { return m_Window; }

	void ToggleCursorVisibility();

	int GetWidth() const { return m_Width; }
	
	int GetHeight() const { return m_Height; }
	
private:
	static void Init();
	static void Shutdown();

private:
	static int s_NumWindows;

	GLFWwindow* m_Window{ nullptr };
	int m_Width{};
	int m_Height{};	
};