#pragma once

#include <cstdint>

enum class KeyCode : uint64_t
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
    MouseRight = 1ul << 8ul,
    Esc = 1ul << 9ul
};

enum class KeyAction
{
    Release,
    Press,
    Repeat
};

struct KeyEvent
{
    KeyCode Code;
    KeyAction Action;
};

struct MouseMoveEvent
{
    float X;
    float Y;
};

class Window;

namespace Input
{
bool IsPressed(KeyCode keyCode);

float GetMousePosX();

float GetMousePosY();

void PollEvents();

void PerTickFlush();

void ActivateWindow(Window* window);

void Reset();
}; // namespace Input