#pragma once

namespace Config
{
	inline constexpr int WindowWidth = 2560;
	inline constexpr int WindowHeight = 1440;
	inline constexpr const char* WindowTitle = "Voxels";
	inline constexpr bool EnableFullscreen = false;
	inline constexpr bool EnableVSync = false;
	inline constexpr int ChunkLoadDistance = 8;
	inline constexpr int ChunkRenderDistance = 8;
	inline constexpr int TickRate = 20;
	inline constexpr float MouseSensitivity = 0.05f;
	inline constexpr float HorizontalFOV = 90.0f;
}