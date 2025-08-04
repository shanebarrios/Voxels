#include "Window.h"

#include "Utils/Logger.h"
#include <array>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Input.h"

int Window::s_NumWindows = 0;

Window::Window(int width, int height, std::string_view name, bool vsync, bool fullscreen) 
    : m_Width{ width }, m_Height{ height }
{
    if (s_NumWindows++ == 0)
    {
        Window::Init();
    }
    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_Window = glfwCreateWindow(width, height, name.data(), monitor, nullptr);
    if (m_Window == nullptr)
    {
        LOG_ERROR("Failed to create window");
        return;
    }
    glfwSetWindowUserPointer(m_Window, this);
    ToggleCursorVisibility();
    Activate();

    if (!vsync) glfwSwapInterval(0);
}

Window::~Window()
{
    if (m_Window != nullptr)
    {
        glfwDestroyWindow(m_Window);
    }
    if (--s_NumWindows == 0)
    {
        Window::Shutdown();
    }
}

Window::Window(Window&& other) noexcept : m_Window{ other.m_Window }, m_Width{ other.m_Width }, m_Height{ other.m_Height }
{
    other.m_Window = nullptr;
    s_NumWindows++;
}

Window& Window::operator=(Window&& other) noexcept
{
    if (&other == this)
    {
        return *this;
    }
    if (m_Window != nullptr)
    {
        glfwDestroyWindow(m_Window);
    }
    m_Window = other.m_Window;
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    s_NumWindows++;
    return *this;
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_Window);
}

void Window::Activate()
{
    glfwMakeContextCurrent(m_Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to load OpenGL functions");
    }
    glViewport(0, 0, m_Width, m_Height);
    Input::ActivateWindow(this);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::Init()
{
    if (!glfwInit())
    {
        LOG_ERROR("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Window::Shutdown()
{
    glfwTerminate();
}

void Window::ToggleCursorVisibility()
{
    const bool visible = glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
    glfwSetInputMode(m_Window, GLFW_CURSOR, visible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}