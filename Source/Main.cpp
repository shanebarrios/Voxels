#include <chrono>
#include <array>
#include <vector>
#include "Utils/Logger.h" // Included before GLFW to avoid APIENTRY redefinition warning
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "World/World.h"
#include "Utils/DebugState.h"
#include "Utils/Config.h"

DebugState g_DebugState{};

// TODO:
// Batch chunk meshes
// Frustum culling
// CSM
// Ambient occlusion
// Better terrain generation

int main()
{
	Logger::Init();
	Window window
	{ 
		Config::WindowWidth, 
		Config::WindowHeight, 
		"Voxels", 
		Config::EnableVSync, 
		Config::EnableFullscreen 
	};

	World world{};

	Camera camera{world.GetPlayerView()};

	Renderer renderer{ Config::WindowWidth, Config::WindowHeight };

	using namespace std::chrono;

	high_resolution_clock::time_point lastFrame = high_resolution_clock::now();
	high_resolution_clock::time_point lastFPSUpdate = lastFrame;
	float lag = 0.0f;

	LOG_INFO("Main loop running");

	while (!window.ShouldClose())
	{
		const high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
		lag += deltaTime;

		if (std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFPSUpdate).count() > 1000)
		{
			LOG_INFO("{}", g_DebugState);
			const glm::vec3 cameraPos = camera.GetPosition();
			LOG_INFO("pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);
			lastFPSUpdate = currentFrame;
			g_DebugState.Reset();
		}
	
		constexpr float tickDelay = 1.0f / Config::TickRate;
		while (lag >= tickDelay)
		{
			world.Update({ window.GetInput(), camera.GetYaw(), camera.GetPitch() });

			camera.Tick();

			lag -= tickDelay;
			g_DebugState.Ticks++;
			window.FlushInput();
		}

		const float alpha = lag / tickDelay;
		camera.Update(window.GetInput(), alpha);

		renderer.Render(world, camera);
		window.Update();
		g_DebugState.Frames++;
	}
}
