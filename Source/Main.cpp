#ifdef _WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif
#include <chrono>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "World/World.h"
#include "ECS/ECS.h"
#include "ECS/Components.h"
#include "Math/Noise.h"

extern float noiseMin;
extern float noiseMax;
extern float octaveMin;
extern float octaveMax;
extern int remeshes;
extern int loaded;

// TODO:
// Batch chunk meshes
// Frustum culling
// CSM
// Ambient occlusion
// Better terrain generation

int main()
{
	Logger::Init();
	Window window{ 2560, 1440, "Voxels" };

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
	std::cout << glGetString(GL_VERSION) << '\n';

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
			LOG_INFO("noise min: {}", noiseMin);
			LOG_INFO("noise max: {}", noiseMax);
			LOG_INFO("octave min: {}", octaveMin);
			LOG_INFO("octave max: {}", octaveMax);
			LOG_INFO("remeshes: {}", remeshes);
			LOG_INFO("loaded: {}", loaded);
			const glm::vec3 cameraPos = camera.GetPosition();
			LOG_INFO("pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);
			loaded = 0;
			remeshes = 0;
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
