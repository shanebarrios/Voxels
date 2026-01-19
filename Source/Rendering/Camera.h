#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "World/Coordinates.h"
#include "World/World.h"
#include "Core/Config.h"

using Entity = uint32_t;

struct Plane
{
    glm::vec3 Normal;
    glm::vec3 P0;
};

class UIOverlay;

class Camera
{
  public:
    static constexpr size_t NUM_CASCADES = 4;

    Camera();
    Camera(const PlayerView& view);

    glm::mat4 GetViewMatrix() const { return m_View; }
    const glm::mat4& GetProjectionMatrix() const { return m_Projection; }
    const std::array<float, NUM_CASCADES + 1>& GetSubfrustaPlaneDepths() const
    {
        return m_SubfrustaPlaneDepths;
    };
    const glm::mat4& GetSubfrustaProjectionMatrix(size_t index) const
    {
        return m_SubfrustaProjectionMatrices[index];
    };

    void GetFrustumPlanes(std::array<Plane, 6>& planes) const;

    void GetFrustumCornersWorldSpace(std::array<glm::vec3, 8>& corners) const;

    void GetSubfrustumCornersWorldSpace(std::array<glm::vec3, 8>& corners,
                                        size_t index) const;

    glm::vec3 GetPosition() const { return m_Pos; }
    glm::vec3 GetDirection() const { return m_Direction; }

    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }

    void Update(float alpha);
    void Tick();

    void AttachView(const PlayerView& view);

    void ToggleControls();

    void SetFOVDegreesHorizontal(float degrees);

    void SetSens(float sense) { m_Sens = sense; }

    friend class UIOverlay;

  private:
    void InitMatrices();

    void UpdateOrientation();

    void UpdateDirection();

  private:
    glm::vec3 m_Pos{};
    glm::vec3 m_Direction{0.0f, 0.0f, -1.0f};

    std::array<float, NUM_CASCADES + 1> m_SubfrustaPlaneDepths;
    std::array<glm::mat4, NUM_CASCADES> m_SubfrustaProjectionMatrices;
    glm::mat4 m_Projection;
    glm::mat4 m_View;

    WorldCoords m_CurTickPosition{};
    WorldCoords m_LastTickPosition{};

    PlayerView m_PlayerView{};

    float m_Sens = Config::MouseSensitivity;
    float m_Fov = Config::HorizontalFOV;
    float m_AspectRatio = static_cast<float>(Config::WindowWidth) /
                          static_cast<float>(Config::WindowHeight);

    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    float m_LastCursorPosX{};
    float m_LastCursorPosY{};

    bool m_First = true;
    bool m_ControlsEnabled = true;
};