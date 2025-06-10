#include <chrono>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "world/World.h"
#include "ecs/ECS.h"
#include "ecs/Components.h"


int main()
{
	Logger::Init();
	Window window{ 1920, 1080, "Voxels" };

	World world{};

	Camera camera{world.GetPlayerView()};

	Renderer renderer{};

	constexpr float tickDelay = 1.0f / 20.0f;

	auto lastFrame = std::chrono::high_resolution_clock::now();
	auto lastFPSUpdate = lastFrame;
	float lag = 0.0f;
	int numFrames = 0;
	int numTicks = 0;


	LOG_INFO("Main loop running");

	while (!window.ShouldClose())
	{
		const auto currentFrame = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
		lag += deltaTime;

		if (std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFPSUpdate).count() > 1000)
		{
			LOG_INFO("fps: {}", numFrames);
			LOG_INFO("tps: {}", numTicks);
			lastFPSUpdate = currentFrame;
			numFrames = 0;
			numTicks = 0;
		}
	
		while (lag >= tickDelay)
		{
			world.Update({ window.GetInput(), camera.GetYaw(), camera.GetPitch() });

			camera.Tick();

			lag -= tickDelay;
			numTicks++;
			window.FlushInput();
		}

		renderer.Clear();

		const float alpha = lag / tickDelay;
		camera.Update(window.GetInput(), alpha);

		renderer.Render(world, camera);
		window.Update();
		numFrames++;
	}
}