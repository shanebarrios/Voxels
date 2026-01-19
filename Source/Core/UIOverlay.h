#pragma once

class Window;
class Camera;
class World;

class UIOverlay
{
  public:
    UIOverlay() {};

    void Init(Window* window, Camera* camera, World* world);

    void Shutdown();

    void BeginRender();
    void EndRender();

    void ToggleVisibility() { m_Enabled = !m_Enabled; }
    bool IsVisible() const { return m_Enabled; }

  private:
    Window* m_Window = nullptr;
    Camera* m_Camera = nullptr;
    World* m_World = nullptr;

    bool m_Enabled = false;
};