#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <cassert>
#include "Window.h"
#include "ECS/Components.h"
#include "Logger.h"

Camera::Camera()
{
    InitMatrices();
}

Camera::Camera(const PlayerView& playerView) :
    m_PlayerView{playerView},
    m_CurTickPosition {playerView.Transform->Position + playerView.Look->Offset},
    m_LastTickPosition {m_CurTickPosition}
{
    InitMatrices();
}

void Camera::InitMatrices()
{
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 27.8f * (k_ChunkViewDistance + 1);
    constexpr float fov = 45.0f;
    constexpr float aspect = 16.0f / 9.0f;

    constexpr float lambda = 0.95f;

    m_SubfrustaPlaneDepths[0] = nearPlane;
    m_SubfrustaPlaneDepths[m_SubfrustaPlaneDepths.size() - 1] = farPlane;
    for (size_t i = 1; i <= k_NumSubdivisions  - 1; i++)
    {
        // Thanks Nvidia (I have no idea what black magic this is)
        const float planeDepth = lambda * nearPlane * std::powf(farPlane / nearPlane, static_cast<float>(i) / k_NumSubdivisions)
            + (1 - lambda) * (nearPlane + (static_cast<float>(i) / k_NumSubdivisions) * (farPlane - nearPlane));
        m_SubfrustaPlaneDepths[i] = planeDepth;
        LOG_INFO("{}", planeDepth);
    }
    for (size_t i = 0; i < k_NumSubdivisions; i++)
    {
        m_SubfrustaProjectionMatrices[i] = glm::perspective(fov, aspect, m_SubfrustaPlaneDepths[i], m_SubfrustaPlaneDepths[i + 1]);
    }

    m_Projection = glm::perspective(fov, aspect, 0.1f, farPlane);
}

void Camera::AttachView(const PlayerView& playerView)
{
    m_PlayerView = playerView;
    m_CurTickPosition = playerView.Transform->Position + playerView.Look->Offset;
    m_LastTickPosition = m_CurTickPosition;
}

void Camera::Update(const Input& input, float alpha)
{
    UpdateOrientation(input);
    m_Pos = static_cast<glm::vec3>(m_LastTickPosition * (1 - alpha) + m_CurTickPosition * alpha);
}

void Camera::Tick()
{
    m_LastTickPosition = m_CurTickPosition;
    m_CurTickPosition = m_PlayerView.Transform->Position + m_PlayerView.Look->Offset;
}

void Camera::UpdateOrientation(const Input& input)
{
    const float cursorPosX = static_cast<float>(input.GetCursorPosX());
    const float cursorPosY = static_cast<float>(input.GetCursorPosY());

    if (m_First)
    {
        m_LastCursorPosX = cursorPosX;
        m_LastCursorPosY = cursorPosY;
        m_First = false;
    }

    const float deltaX = cursorPosX - m_LastCursorPosX;
    const float deltaY = cursorPosY - m_LastCursorPosY;

    m_LastCursorPosX = cursorPosX;
    m_LastCursorPosY = cursorPosY;

    m_Pitch += -deltaY * m_Sens;
    m_Yaw += deltaX * m_Sens;
    if (m_Pitch < -89.0f)
    {
        m_Pitch = -89.0f;
    }
    if (m_Pitch > 89.0f)
    {
        m_Pitch = 89.0f;
    }
    UpdateDirection();
}

void Camera::UpdateDirection()
{
    m_Direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Direction.y = sin(glm::radians(m_Pitch));
    m_Direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
}
