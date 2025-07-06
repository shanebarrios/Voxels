#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <cassert>
#include "Window.h"
#include "ECS/Components.h"

Camera::Camera(const PlayerView& playerView) :
    m_PlayerView{playerView},
    m_CurTickPosition {playerView.Transform->Position + playerView.Look->Offset},
    m_LastTickPosition {m_CurTickPosition}
{

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
