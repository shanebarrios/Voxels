#include "Application.h"
#include "Core/Config.h"
#include "Core/DebugState.h"
#include "Core/Input.h"
#include "Core/Logger.h"
#include "Math/MathUtils.h"

DebugState g_DebugState{};

static Application* s_Instance = nullptr;

Application::Application()
    : m_Window{Config::WindowWidth, Config::WindowHeight, Config::WindowTitle,
               Config::EnableVSync, Config::EnableFullscreen},
      m_World{}, m_Camera{},
      m_Renderer{Config::WindowWidth, Config::WindowHeight}, m_UIOverlay{}
{
    s_Instance = this;
    g_Logger.Init();
    g_ChunkAllocator.Init(MathUtils::Cube(Config::ChunkLoadDistance + 1) * 16);
    m_World.Init();
    m_Camera.AttachView(m_World.GetPlayerView());
    m_UIOverlay.Init(&m_Window, &m_Camera, &m_World);

    LOG_INFO("Application initialized");
}

Application::~Application()
{
    LOG_INFO("Application shutting down");

    g_ChunkAllocator.Free();
    m_UIOverlay.Shutdown();

    s_Instance = nullptr;
}

Application* Application::Instance()
{
    return s_Instance;
}

void Application::Run()
{
    m_Running = true;
    using namespace std::chrono;
    high_resolution_clock::time_point lastFrame = high_resolution_clock::now();
    high_resolution_clock::time_point lastSecond = lastFrame;
    float lag = 0.0f;
    LOG_INFO("Main loop running");
    while (m_Running)
    {
        const high_resolution_clock::time_point currentFrame =
            high_resolution_clock::now();
        const float deltaTime =
            duration<float>(currentFrame - lastFrame).count();
        lastFrame = currentFrame;
        lag += deltaTime;
        if (duration_cast<milliseconds>(currentFrame - lastSecond).count() >
            1000)
        {
            OnSecondElapsed();
            lastSecond += seconds(1);
        }
        constexpr float tickDelay = 1.0f / Config::TickRate;
        while (lag >= tickDelay)
        {
            OnTick();
            lag -= tickDelay;
        }

        const float alpha = lag / tickDelay;
        OnFrame(deltaTime, alpha);
        if (m_Window.ShouldClose())
        {
            m_Running = false;
        }
    }
}

void Application::OnFrame(float deltaTime, float alpha)
{
    Input::PollEvents();

    m_Camera.Update(alpha);

    m_UIOverlay.BeginRender();
    m_Renderer.Render(m_World, m_Camera);
    m_UIOverlay.EndRender();

    m_Window.SwapBuffers();
    g_DebugState.Frames++;
}

void Application::OnTick()
{
    m_World.Update(m_Camera);

    m_Camera.Tick();

    g_DebugState.Ticks++;
    Input::PerTickFlush();
}

void Application::OnSecondElapsed()
{
    LOG_INFO("{}", g_DebugState);
    const glm::vec3 cameraPos = m_Camera.GetPosition();
    LOG_INFO("pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);
    g_DebugState.Reset();
}

void Application::OnKeyEvent(KeyEvent event)
{
    if (event.Action == KeyAction::Press &&
        (event.Code == KeyCode::Esc || m_UIOverlay.IsVisible()))
    {
        Input::Reset();
        m_UIOverlay.ToggleVisibility();
        m_Window.ToggleCursorVisibility();
        m_World.TogglePlayerController();
        m_Camera.ToggleControls();
    }
}

void Application::OnMouseMoveEvent(MouseMoveEvent event) {}
