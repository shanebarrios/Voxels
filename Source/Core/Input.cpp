#include "Input.h"

#include "Application.h"
#include <GLFW/glfw3.h>
#include <array>
#include "Window.h"
#include "imgui.h"

static Window* s_ActiveWindow = nullptr;
static uint64_t s_PressedKeys = 0;
static float s_MousePosX = 0.0f;
static float s_MousePosY = 0.0f;

static constexpr std::array<KeyCode, GLFW_KEY_LAST + 1> GenerateKeyMappings()
{
    std::array<KeyCode, GLFW_KEY_LAST + 1> mappings{};
    mappings[GLFW_MOUSE_BUTTON_LEFT] = KeyCode::MouseLeft;
    mappings[GLFW_MOUSE_BUTTON_RIGHT] = KeyCode::MouseRight;
    mappings[GLFW_KEY_W] = KeyCode::W;
    mappings[GLFW_KEY_A] = KeyCode::A;
    mappings[GLFW_KEY_S] = KeyCode::S;
    mappings[GLFW_KEY_D] = KeyCode::D;
    mappings[GLFW_KEY_SPACE] = KeyCode::Space;
    mappings[GLFW_KEY_LEFT_CONTROL] = KeyCode::Ctrl;
    mappings[GLFW_KEY_LEFT_SHIFT] = KeyCode::Shift;
    mappings[GLFW_KEY_ESCAPE] = KeyCode::Esc;
    return mappings;
}

static KeyCode GetKeyCodeMapping(int glfwButton)
{
    static constexpr std::array<KeyCode, GLFW_KEY_LAST + 1> mappings =
        GenerateKeyMappings();

    return mappings[static_cast<size_t>(glfwButton)];
}

static KeyAction GetKeyActionMapping(int glfwAction)
{
    switch (glfwAction)
    {
    case GLFW_PRESS: return KeyAction::Press;
    case GLFW_RELEASE: return KeyAction::Release;
    case GLFW_REPEAT: return KeyAction::Repeat;
    default: return KeyAction::Release;
    }
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    const KeyCode keyMapping = GetKeyCodeMapping(button);
    if (action == GLFW_PRESS)
    {
        s_PressedKeys |= static_cast<uint64_t>(keyMapping);
    }
    Application::Instance()->OnKeyEvent(
        {keyMapping, GetKeyActionMapping(action)});
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    if (key < 0 || key > GLFW_KEY_LAST)
        return;

    const KeyCode keyMapping = GetKeyCodeMapping(key);
    ;
    if (action == GLFW_PRESS)
    {
        s_PressedKeys |= static_cast<uint64_t>(keyMapping);
    }
    else if (action == GLFW_RELEASE)
    {
        s_PressedKeys &= ~static_cast<uint64_t>(keyMapping);
    }
    Application::Instance()->OnKeyEvent(
        {keyMapping, GetKeyActionMapping(action)});
}

static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    s_MousePosX = static_cast<float>(xpos);
    s_MousePosY = static_cast<float>(ypos);
    Application::Instance()->OnMouseMoveEvent({s_MousePosX, s_MousePosY});
}

namespace Input
{
bool IsPressed(KeyCode keyCode)
{
    return s_PressedKeys & static_cast<uint64_t>(keyCode);
}

float GetMousePosX()
{
    return s_MousePosX;
}

float GetMousePosY()
{
    return s_MousePosY;
}

void PollEvents()
{
    glfwPollEvents();
}

void ActivateWindow(Window* window)
{
    if (s_ActiveWindow)
    {
        GLFWwindow* prev = s_ActiveWindow->GetHandle();
        glfwSetMouseButtonCallback(prev, nullptr);
        glfwSetCursorPosCallback(prev, nullptr);
        glfwSetKeyCallback(prev, nullptr);
    }
    if (!window)
        return;

    GLFWwindow* handle = window->GetHandle();
    glfwSetMouseButtonCallback(handle, MouseButtonCallback);
    glfwSetCursorPosCallback(handle, CursorPosCallback);
    glfwSetKeyCallback(handle, KeyCallback);
    double posX, posY;
    glfwGetCursorPos(handle, &posX, &posY);
    s_MousePosX = posX;
    s_MousePosY = posY;
    s_ActiveWindow = window;
}

void PerTickFlush()
{
    s_PressedKeys &= ~(static_cast<uint64_t>(KeyCode::MouseLeft) |
                       static_cast<uint64_t>(KeyCode::MouseRight));
}

void Reset()
{
    s_PressedKeys = 0;
}
} // namespace Input