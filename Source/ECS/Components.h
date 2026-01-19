#pragma once

#include "World/Coordinates.h"
#include <tuple>

enum ComponentID : uint32_t
{
    ID_Transform,
    ID_Physics,
    ID_Input,
    ID_Look,
    ID_MaxComponents
};

template <typename T>
concept Component = requires {
    { T::ID } -> std::convertible_to<ComponentID>;
    { T::MaxInstances } -> std::convertible_to<size_t>;
};

#define DEFAULT_MAX_INSTANCES 8192
inline constexpr size_t MAX_ENTITIES = DEFAULT_MAX_INSTANCES;

struct TransformComponent
{
    static constexpr ComponentID ID = ID_Transform;
    static constexpr size_t MaxInstances = DEFAULT_MAX_INSTANCES;
    WorldCoords Position;
    float Yaw;
};

struct Collider
{
    WorldCoords Min;
    WorldCoords Max;
};

struct PhysicsComponent
{
    static constexpr ComponentID ID = ID_Physics;
    static constexpr size_t MaxInstances = DEFAULT_MAX_INSTANCES;
    WorldCoords Velocity;
    Collider Collider;
    bool Airborne;
};

struct InputComponent
{
    static constexpr ComponentID ID = ID_Input;
    static constexpr size_t MaxInstances = DEFAULT_MAX_INSTANCES;
    float MoveX;
    float MoveZ;
    uint32_t InputFlags;

    enum InputFlag : uint32_t
    {
        Jump = 1u,
        Sprint = 1u << 1u,
        Crouch = 1u << 2u,
        Attack = 1u << 3u,
        Interact = 1u << 4u
    };
};

struct LookComponent
{
    static constexpr ComponentID ID = ID_Look;
    static constexpr size_t MaxInstances = DEFAULT_MAX_INSTANCES;
    WorldCoords Offset;
    float Yaw;
    float Pitch;
};

using AllComponents = std::tuple<TransformComponent, PhysicsComponent,
                                 InputComponent, LookComponent>;
