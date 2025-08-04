#include "UIOverlay.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Window.h"
#include "World/World.h"
#include "Rendering/Camera.h"
#include "Utils/Coordinates.h"
#include "ECS/Components.h"
#include "ECS/ECS.h"
#include "World/Block.h"

void UIOverlay::Init(
	Window* window,
	Camera* camera,
	World* world)
{
	m_Window = window;
	m_Camera = camera;
	m_World = world;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
	ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), true);
	ImGui_ImplOpenGL3_Init();
}

void UIOverlay::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIOverlay::BeginRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (!m_Enabled) return;


	ImGui::Begin("Editor");
	if (ImGui::CollapsingHeader("Player"))
	{
		const WorldCoords playerPos = m_World->GetPlayerView().Transform->Position;
		const ChunkCoords playerPosChunk = static_cast<ChunkCoords>(playerPos);
		ImGui::Text("World pos: (%f, %f, %f)", playerPos.X, playerPos.Y, playerPos.Z);
		ImGui::Text("Chunk pos: (%d, %d, %d)", playerPosChunk.X, playerPosChunk.Y, playerPosChunk.Z);
		bool enablePhysics = m_World->IsPlayerPhysicsEnabled();
		if (ImGui::Checkbox("Enable physics", &enablePhysics))
		{
			m_World->SetPlayerPhysics(enablePhysics);
		}
		// Some weird arithmetic here so that we can't set air blocks
		const char** blocks = GetAllBlockTypeNames() + 1;
		int option = static_cast<int>(m_World->GetPlayerActiveBlock()) - 1;
		if (ImGui::Combo("Active block", &option, blocks, static_cast<int>(BlockType::NumBlockTypes) - 1))
		{
			m_World->SetPlayerActiveBlock(static_cast<BlockType>(option + 1));
		}
	}
	if (ImGui::CollapsingHeader("Camera"))
	{
		float fov = m_Camera->m_Fov;
		if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f, "%.1f"))
		{
			m_Camera->SetFOVDegreesHorizontal(fov);
		}
		float sens = m_Camera->m_Sens * 50;
		if (ImGui::SliderFloat("Mouse sensitivity", &sens, 0.1f, 10.0f, "%.1f"))
		{
			m_Camera->SetSens(sens / 50);
		}
	}
	if (ImGui::CollapsingHeader("Light"))
	{

	}
	ImGui::End();
}

void UIOverlay::EndRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}