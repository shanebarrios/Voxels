#pragma once

#include "UIOverlay.h"
#include "Window.h"
#include "World/World.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Utils/Config.h"
#include "Input.h"

class Application
{
public:
	static Application* Instance();

	Application();
	~Application();

	void Run();

	void OnKeyEvent(KeyEvent event);

	void OnMouseMoveEvent(MouseMoveEvent event);

private:
	void OnFrame(float deltaTime, float alpha);
	void OnTick();
	void OnSecondElapsed();

private:
	Window m_Window;
	World m_World;
	Camera m_Camera;
	Renderer m_Renderer;
	UIOverlay m_UIOverlay;

	bool m_Running = false;
};
