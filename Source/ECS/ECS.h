#pragma once

#include <array>
#include <vector>

#include "ComponentArray.h"
#include "Components.h"
#include "Memory/ArenaAllocator.h"

class ECS
{
  public:
    ECS() = default;

    void Init();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    template <Component C>
    C& AddComponent(Entity entity, C&& component);

    template <Component C>
    void RemoveComponent(Entity entity);

    template <Component C>
    C& GetComponent(Entity entity);

    template <Component C>
    const C& GetComponent(Entity entity) const;

    template <Component C>
    C* GetOptionalComponent(Entity entity);

    template <Component C>
    const C* GetOptionalComponent(Entity entity) const;

    template <Component C>
    bool HasComponent(Entity entity) const;

    template <Component C>
    ComponentArray<C>& GetComponentArray();

    template <Component C>
    const ComponentArray<C>& GetComponentArray() const;

  private:
    template <Component C>
    void AllocateComponentArray();

  private:
    Entity m_EntityCount = 0;
    ArenaAllocator m_ArenaAllocator{};

    std::array<void*, ID_MaxComponents> m_ComponentArrays{};
    std::vector<Entity> m_FreedList{};
};

namespace detail
{
template <Component... Cs>
constexpr size_t TotalArenaSize()
{
    return (0 + ... + sizeof(ComponentArray<Cs>));
}
} // namespace detail

inline void ECS::Init()
{
    constexpr size_t arenaSize = std::apply(
        [](auto... components) {
            return (0 + ... + sizeof(ComponentArray<decltype(components)>));
        },
        AllComponents{});
    m_ArenaAllocator.Init(arenaSize, 0);
    std::apply(
        [this](auto... components) {
            (this->AllocateComponentArray<decltype(components)>(), ...);
        },
        AllComponents{});
}

template <Component C>
void ECS::AllocateComponentArray()
{
    auto ptr = m_ArenaAllocator.New<ComponentArray<C>>();
    m_ComponentArrays[C::ID] = ptr;
}

template <Component C>
inline ComponentArray<C>& ECS::GetComponentArray()
{
    constexpr ComponentID id = C::ID;
    void* const mem = m_ComponentArrays[id];
    return *reinterpret_cast<ComponentArray<C>*>(mem);
}

template <Component C>
inline const ComponentArray<C>& ECS::GetComponentArray() const
{
    constexpr ComponentID id = C::ID;
    const void* mem = m_ComponentArrays[id];
    return *reinterpret_cast<const ComponentArray<C>*>(mem);
}

inline Entity ECS::CreateEntity()
{
    if (!m_FreedList.empty())
    {
        Entity entity = m_FreedList.back();
        m_FreedList.pop_back();
        return entity;
    }
    else
    {
        assert(m_EntityCount < MAX_ENTITIES);
        return m_EntityCount++;
    }
}

inline void ECS::DestroyEntity(Entity entity)
{
    assert(entity < MAX_ENTITIES);
    std::apply(
        [this, entity](auto... components) {
            (this->RemoveComponent<decltype(components)>(entity), ...);
        },
        AllComponents{});
    m_FreedList.push_back(entity);
}

template <Component C>
inline C& ECS::AddComponent(Entity entity, C&& component)
{
    return GetComponentArray<C>().Insert(entity, std::move(component));
}

template <Component C>
inline void ECS::RemoveComponent(Entity entity)
{
    GetComponentArray<C>().Remove(entity);
}

template <Component C>
inline C& ECS::GetComponent(Entity entity)
{
    return GetComponentArray<C>().Get(entity);
}

template <Component C>
inline const C& ECS::GetComponent(Entity entity) const
{
    return GetComponentArray<C>().Get(entity);
}

template <Component C>
inline C* ECS::GetOptionalComponent(Entity entity)
{
    return GetComponentArray<C>().OptionalGet(entity);
}

template <Component C>
const C* ECS::GetOptionalComponent(Entity entity) const
{
    return GetComponentArray<C>().OptionalGet(entity);
}

template <Component C>
inline bool ECS::HasComponent(Entity entity) const
{
    return GetComponentArray<C>().Contains(entity);
}
