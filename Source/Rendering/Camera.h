#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Coordinates.h"
#include "World/World.h"

using Entity = uint32_t;

class Input;

class Camera {
public:
    static constexpr int k_ChunkViewDistance = 16;
    static constexpr int k_NumSubdivisions = 4;

    Camera();
    Camera(const PlayerView& view);

    glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Pos, m_Pos + m_Direction, glm::vec3{ 0.0f, 1.0f, 0.0f }); }
    const glm::mat4& GetProjectionMatrix() const { return m_Projection; }
    const std::array<float, k_NumSubdivisions + 1>& GetSubfrustaPlaneDepths() const { return m_SubfrustaPlaneDepths; };
    const glm::mat4& GetSubfrustaProjectionMatrix(size_t index) const { return m_SubfrustaProjectionMatrices[index]; };

    glm::vec3 GetPosition() const { return m_Pos; }
    glm::vec3 GetDirection() const { return m_Direction; }

    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    
    void Update(const Input& input, float alpha);
    void Tick();

    void AttachView(const PlayerView& view);

private:
    glm::vec3 m_Pos {};
    glm::vec3 m_Direction { 0.0f, 0.0f, -1.0f };

    std::array<float, k_NumSubdivisions + 1> m_SubfrustaPlaneDepths;
    std::array<glm::mat4, k_NumSubdivisions> m_SubfrustaProjectionMatrices;
    glm::mat4 m_Projection;

    WorldCoords m_CurTickPosition{};
    WorldCoords m_LastTickPosition{};

    PlayerView m_PlayerView {};

    float m_Sens = 0.05f;

    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    float m_LastCursorPosX{};
    float m_LastCursorPosY{};

    bool m_First = true;

    void InitMatrices();

    void UpdateOrientation(const Input& input);

    void UpdateDirection();
};