#pragma once

#include <string_view>

enum class Key : uint64_t
{
	None = 0ul,
	W = 1ul,
	A = 1ul << 1ul,
	S = 1ul << 2ul,
	D = 1ul << 3ul,
	Space = 1ul << 4ul,
	Ctrl = 1ul << 5ul,
	Shift = 1ul << 6ul,
	MouseLeft = 1ul << 7ul,
	MouseRight = 1ul << 8ul
};

class Window;

class Input
{
public:
	Input() = default;
	double GetCursorPosX() const;
	double GetCursorPosY() const;
	bool IsPressed(Key key) const;

private:
	double m_CursorPosX;
	double m_CursorPosY;
	// could be smaller but its not even a kilobyte who cares
	uint64_t m_Keys;

	friend class Window;
};

struct GLFWwindow;

class Window
{
public:
	Window(int width, int height, std::string_view name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) noexcept;
	Window& operator=(Window&&) noexcept;

	void Update();
	void FlushInput();

	void Activate() const;
	bool ShouldClose() const;

	const Input& GetInput() const { return m_Input; }

private:
	static int s_NumWindows;
	static void Init();
	static void Destroy();

	static void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CursorCallback(GLFWwindow* window, double xpos, double ypos);


	GLFWwindow* m_Window{ nullptr };
	int m_Width{};
	int m_Height{};
	
	Input m_Input{};
};