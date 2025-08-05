#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <cassert>
#include "Input.h"
#include "ECS/Components.h"
#include "Utils/Logger.h"

static float HorizontalToVerticalFov(float horizontalFov, float aspectRatio)
{
	return 2.0f * glm::atan(glm::tan(horizontalFov / 2.0f) / aspectRatio);
}

enum FrustumCorners
{
    NEAR_BOTTOM_LEFT = 0,
    NEAR_BOTTOM_RIGHT,
    NEAR_TOP_LEFT,
    NEAR_TOP_RIGHT,
    FAR_BOTTOM_LEFT,
    FAR_BOTTOM_RIGHT,
    FAR_TOP_LEFT,
    FAR_TOP_RIGHT
};

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
    constexpr float farPlane = 27.8f * (Config::ChunkRenderDistance * 2 + 1);

    constexpr float lambda = 0.8f;

	const float fov = HorizontalToVerticalFov(glm::radians(m_Fov), m_AspectRatio);

    m_SubfrustaPlaneDepths[0] = nearPlane;
    m_SubfrustaPlaneDepths[NUM_CASCADES] = farPlane;
    for (size_t i = 1; i < NUM_CASCADES; i++)
    {
        // Thanks Nvidia (I have no idea what black magic this is)
        const float planeDepth = lambda * nearPlane * std::powf(farPlane / nearPlane, static_cast<float>(i) / NUM_CASCADES)
            + (1 - lambda) * (nearPlane + (static_cast<float>(i) / NUM_CASCADES) * (farPlane - nearPlane));
        m_SubfrustaPlaneDepths[i] = planeDepth;
        LOG_INFO("{}", planeDepth);
    }
    for (size_t i = 0; i < NUM_CASCADES; i++)
    {
        m_SubfrustaProjectionMatrices[i] = glm::perspective(fov, m_AspectRatio, m_SubfrustaPlaneDepths[i], m_SubfrustaPlaneDepths[i + 1]);
    }

    m_Projection = glm::perspective(fov, m_AspectRatio, nearPlane, farPlane);
    m_View = glm::lookAt(m_Pos, m_Pos + m_Direction, glm::vec3{ 0.0f, 1.0f, 0.0f });
}

void Camera::AttachView(const PlayerView& playerView)
{
    m_PlayerView = playerView;
    m_CurTickPosition = playerView.Transform->Position + playerView.Look->Offset;
    m_LastTickPosition = m_CurTickPosition;
}

void Camera::ToggleControls()
{
	m_ControlsEnabled = !m_ControlsEnabled;
    if (m_ControlsEnabled)
    {
        m_First = true;
    }
}

void Camera::SetFOVDegreesHorizontal(float degrees)
{
    m_Fov = degrees;
	const float fov = HorizontalToVerticalFov(glm::radians(m_Fov), m_AspectRatio);
    m_Projection = glm::perspective(
        fov, m_AspectRatio, m_SubfrustaPlaneDepths[0], m_SubfrustaPlaneDepths[NUM_CASCADES]
    );
    for (size_t i = 0; i < NUM_CASCADES; i++)
    {
        m_SubfrustaProjectionMatrices[i] = glm::perspective(fov, m_AspectRatio, m_SubfrustaPlaneDepths[i], m_SubfrustaPlaneDepths[i + 1]);
    }
}

void Camera::Update(float alpha)
{ 
	if (m_ControlsEnabled)
        UpdateOrientation();
    m_Pos = static_cast<glm::vec3>(m_LastTickPosition * (1 - alpha) + m_CurTickPosition * alpha);
    m_View = glm::lookAt(m_Pos, m_Pos + m_Direction, glm::vec3{ 0.0f, 1.0f, 0.0f });
}

void Camera::GetFrustumPlanes(std::array<Plane, 6>& planes) const
{
    std::array<glm::vec3, 8> corners;
    GetFrustumCornersWorldSpace(corners);

    const glm::vec3 frontN = m_Direction;

    const glm::vec3 backN = -m_Direction;

    const glm::vec3 leftN = glm::normalize(
        glm::cross(
            corners[FAR_BOTTOM_LEFT] - corners[NEAR_BOTTOM_LEFT],
            corners[NEAR_TOP_LEFT] - corners[NEAR_BOTTOM_LEFT]
        )
    );

    const glm::vec3 rightN = glm::normalize(
        glm::cross(
            corners[NEAR_TOP_RIGHT] - corners[NEAR_BOTTOM_RIGHT], 
            corners[FAR_BOTTOM_RIGHT] - corners[NEAR_BOTTOM_RIGHT]
        )
    );

    const glm::vec3 upN = glm::normalize(
        glm::cross(
            corners[FAR_TOP_LEFT] - corners[NEAR_TOP_LEFT], 
            corners[NEAR_TOP_RIGHT] - corners[NEAR_TOP_LEFT]
        )
    );

    const glm::vec3 downN = glm::normalize(
        glm::cross(
            corners[NEAR_BOTTOM_RIGHT] - corners[NEAR_BOTTOM_LEFT], 
            corners[FAR_BOTTOM_LEFT] - corners[NEAR_BOTTOM_LEFT]
        )
    );

    planes[0] = Plane{ frontN, corners[NEAR_BOTTOM_LEFT]};
    planes[1] = Plane{ backN, corners[FAR_BOTTOM_LEFT]};
    planes[2] = Plane{ leftN, corners[NEAR_BOTTOM_LEFT]};
    planes[3] = Plane{ rightN, corners[NEAR_BOTTOM_RIGHT]};
    planes[4] = Plane{ upN, corners[NEAR_TOP_LEFT]};
    planes[5] = Plane{ downN, corners[NEAR_BOTTOM_LEFT]};
}

void Camera::GetFrustumCornersWorldSpace(std::array<glm::vec3, 8>& corners) const
{
    const glm::mat4 inverse = glm::inverse(m_Projection * m_View);
    int i = 0;
    for (int z = -1; z <= 1; z += 2)
    {
        for (int y = -1; y <= 1; y += 2)
        {
            for (int x = -1; x <= 1; x += 2)
            {
                const glm::vec4 res = inverse * glm::vec4{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f };
                corners[i++] = glm::vec3{ res / res.w };
            }
        }
    }
}

void Camera::GetSubfrustumCornersWorldSpace(std::array<glm::vec3, 8>& corners, size_t index) const
{
    const glm::mat4 inverse = glm::inverse(m_SubfrustaProjectionMatrices[index] * m_View);
    int i = 0;
    for (int z = -1; z <= 1; z += 2)
    {
        for (int y = -1; y <= 1; y += 2)
        {
            for (int x = -1; x <= 1; x += 2)
            {
                const glm::vec4 res = inverse * glm::vec4{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f };
                corners[i++] = glm::vec3{ res / res.w };
            }
        }
    }
}

void Camera::Tick()
{
    m_LastTickPosition = m_CurTickPosition;
    m_CurTickPosition = m_PlayerView.Transform->Position + m_PlayerView.Look->Offset;
}

void Camera::UpdateOrientation()
{
    const float cursorPosX = Input::GetMousePosX();
    const float cursorPosY = Input::GetMousePosY();

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
