#include "Window.h"

#include <array>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Logger.h"

static constexpr std::array<Key, GLFW_KEY_LAST + 1> GenerateKeyMappings()
{
    std::array<Key, GLFW_KEY_LAST + 1> mappings{};
    mappings[GLFW_MOUSE_BUTTON_LEFT] = Key::MouseLeft;
    mappings[GLFW_MOUSE_BUTTON_RIGHT] = Key::MouseRight;
    mappings[GLFW_KEY_W] = Key::W;
    mappings[GLFW_KEY_A] = Key::A;
    mappings[GLFW_KEY_S] = Key::S;
    mappings[GLFW_KEY_D] = Key::D;
    mappings[GLFW_KEY_SPACE] = Key::Space;
    mappings[GLFW_KEY_LEFT_CONTROL] = Key::Ctrl;
    mappings[GLFW_KEY_LEFT_SHIFT] = Key::Shift;
    return mappings;
}

static constexpr std::array<Key, GLFW_KEY_LAST + 1> k_GlfwKeyMappings = GenerateKeyMappings();


bool Input::IsPressed(Key key) const
{
    return m_Keys & static_cast<uint64_t>(key);
}

double Input::GetCursorPosX() const
{
    return m_CursorPosX;
}

double Input::GetCursorPosY() const
{
    return m_CursorPosY;
}


int Window::s_NumWindows = 0;

Window::Window(int width, int height, std::string_view name) : m_Width{ width }, m_Height{ height }
{
    if (s_NumWindows++ == 0)
    {
        Window::Init();
    }
    m_Window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
    if (m_Window == nullptr)
    {
        LOG_ERROR("Failed to create window");
    }
    Activate();
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetMouseButtonCallback(m_Window, MouseCallback);
    glfwSetKeyCallback(m_Window, KeyCallback);
    glfwSetCursorPosCallback(m_Window, CursorCallback);
    glfwGetCursorPos(m_Window, &m_Input.m_CursorPosX, &m_Input.m_CursorPosY);
    glfwSwapInterval(0);
}

Window::~Window()
{
    if (m_Window != nullptr)
    {
        glfwDestroyWindow(m_Window);
    }
    if (--s_NumWindows == 0)
    {
        Window::Destroy();
    }
}

Window::Window(Window&& other) noexcept : m_Window{ other.m_Window }, m_Width{ other.m_Width }, m_Height{ other.m_Height }, m_Input{ other.m_Input }
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
    m_Input = other.m_Input;
    s_NumWindows++;
    return *this;
}

void Window::Update()
{
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_Window, true);
    }
}

void Window::FlushInput()
{
    if (1)
    {
        m_Input.m_Keys &= ~static_cast<uint64_t>(k_GlfwKeyMappings[GLFW_MOUSE_BUTTON_LEFT]);
    }
    if (1)
    {
        m_Input.m_Keys &= ~static_cast<uint64_t>(k_GlfwKeyMappings[GLFW_MOUSE_BUTTON_RIGHT]);
    }
}

void Window::Activate() const
{
    glfwMakeContextCurrent(m_Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to load OpenGL functions");
    }
    glViewport(0, 0, m_Width, m_Height);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Window::Destroy()
{
    glfwTerminate();
}

void Window::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    Window* _window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    const Key mapping = k_GlfwKeyMappings[button];
    if (action == GLFW_PRESS)
    {
        _window->m_Input.m_Keys |= static_cast<uint64_t>(mapping);
    }
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0 || key > GLFW_KEY_LAST) return;

    Window* _window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    const Key keyMapping = k_GlfwKeyMappings[key];
    if (action == GLFW_PRESS)
    {
        _window->m_Input.m_Keys |= static_cast<uint64_t>(keyMapping);
    }
    else if (action == GLFW_RELEASE)
    {
        _window->m_Input.m_Keys &= ~static_cast<uint64_t>(keyMapping);
    }
}

void Window::CursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    Window* _window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    _window->m_Input.m_CursorPosX = xpos;
    _window->m_Input.m_CursorPosY = ypos;
}