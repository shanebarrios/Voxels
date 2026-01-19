#pragma once

#include "Components.h"
#include <array>
#include <cassert>
#include <utility>

using Entity = uint32_t;

template <Component C>
class ComponentArray
{
  public:
    ComponentArray();

    void Destroy(Entity entity) { Remove(entity); }

    C& Insert(Entity entity, C&& component);

    C& Get(Entity entity);

    const C& Get(Entity entity) const;

    C* OptionalGet(Entity entity);

    const C* OptionalGet(Entity entity) const;

    void Remove(Entity entity);

    bool Contains(Entity entity) const;

    size_t Count() const { return m_Count; }

    void Clear() { m_Count = 0; };

    template <bool IsConst>
    class Iterator
    {
      public:
        using ComponentType = std::conditional_t<IsConst, const C, C>;
        using EntityType = std::conditional_t<IsConst, const Entity, Entity>;

        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<EntityType, ComponentType&>;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = value_type;

        Iterator(ComponentType* componentIt, EntityType* entityIt)
            : m_ComponentIt{componentIt}, m_EntityIt{entityIt}
        {
        }

        Iterator& operator++()
        {
            m_ComponentIt++;
            m_EntityIt++;
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return m_EntityIt != other.m_EntityIt;
        }

        reference operator*() { return {*m_EntityIt, *m_ComponentIt}; }

      private:
        ComponentType* m_ComponentIt;
        EntityType* m_EntityIt;
    };

    using MutableIterator = Iterator<false>;
    using ConstIterator = Iterator<true>;

    MutableIterator begin() { return {m_Components.data(), m_Dense.data()}; }
    MutableIterator end()
    {
        return {m_Components.data() + m_Count, m_Dense.data() + m_Count};
    }

    ConstIterator begin() const
    {
        return {m_Components.data(), m_Dense.data()};
    }
    ConstIterator end() const
    {
        return {m_Components.data() + m_Count, m_Dense.data() + m_Count};
    }

  private:
    std::array<C, C::MaxInstances> m_Components{};
    std::array<Entity, C::MaxInstances> m_Dense{};
    std::array<size_t, MAX_ENTITIES> m_Sparse;

    size_t m_Count = 0;
};

template <Component C>
inline ComponentArray<C>::ComponentArray()
{
    m_Sparse.fill(MAX_ENTITIES);
}

template <Component C>
inline C& ComponentArray<C>::Insert(Entity entity, C&& component)
{
    assert(m_Count < C::MaxInstances);
    m_Dense[m_Count] = entity;
    m_Components[m_Count] = std::move(component);
    m_Sparse[entity] = m_Count;
    m_Count++;
    return m_Components[m_Count - 1];
}

template <Component C>
inline C& ComponentArray<C>::Get(Entity entity)
{
    assert(Contains(entity));
    return m_Components[m_Sparse[entity]];
}

template <Component C>
inline const C& ComponentArray<C>::Get(Entity entity) const
{
    assert(Contains(entity));
    return m_Components[m_Sparse[entity]];
}

template <Component C>
inline C* ComponentArray<C>::OptionalGet(Entity entity)
{
    if (!Contains(entity))
        return nullptr;
    return &m_Components[m_Sparse[entity]];
}

template <Component C>
inline const C* ComponentArray<C>::OptionalGet(Entity entity) const
{
    if (!Contains(entity))
        return nullptr;
    return &m_Components[m_Sparse[entity]];
}

template <Component C>
inline void ComponentArray<C>::Remove(Entity entity)
{
    const size_t i = m_Sparse[entity];
    if (i >= m_Count || m_Dense[i] != entity)
    {
        return;
    }
    const Entity toSwap = m_Dense[m_Count - 1];
    m_Dense[i] = toSwap;
    m_Components[i] = std::move(m_Components[m_Count - 1]);
    m_Sparse[toSwap] = i;

    m_Count--;
}

template <Component C>
inline bool ComponentArray<C>::Contains(Entity entity) const
{
    const size_t i = m_Sparse[entity];
    return (i < m_Count && m_Dense[i] == entity);
}
