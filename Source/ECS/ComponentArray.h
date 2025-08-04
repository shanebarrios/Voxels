#pragma once

#include <array>
#include <cassert>
#include <utility>
#include <initializer_list>

using Entity = uint32_t;

class IComponentArray {
public:
	virtual ~IComponentArray() = default;

	virtual void Destroy(Entity entity) = 0;
};

template <typename Component, size_t N = 8192>
class ComponentArray : public IComponentArray
{
public:
	ComponentArray();

	void Destroy(Entity entity) override { Remove(entity); }

	Component& Insert(Entity entity, Component&& component);

	Component& Get(Entity entity);

	const Component& Get(Entity entity) const;

	Component* OptionalGet(Entity entity);

	const Component* OptionalGet(Entity entity) const;

	void Remove(Entity entity);

	bool Contains(Entity entity) const;

	size_t Count() const { return m_Count; }

	void Clear() { m_Count = 0; };

	template <bool IsConst>
	class Iterator
	{
	public:
		using ComponentType = std::conditional_t<IsConst, const Component, Component>;
		using EntityType = std::conditional_t<IsConst, const Entity, Entity>;

		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<EntityType, ComponentType&>;
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using reference = value_type;

		Iterator(ComponentType* componentIt, EntityType* entityIt) :
			m_ComponentIt{ componentIt },
			m_EntityIt{ entityIt }
		{}

		Iterator& operator++() { m_ComponentIt++; m_EntityIt++; return *this;  }

		bool operator!=(const Iterator& other) const { return m_EntityIt != other.m_EntityIt; }

		reference operator*() { return { *m_EntityIt, *m_ComponentIt }; }

	private:
		ComponentType* m_ComponentIt;
		EntityType* m_EntityIt;
	};

	using MutableIterator = Iterator<false>;
	using ConstIterator = Iterator<true>;

	MutableIterator begin() { return { m_Components.data(), m_Dense.data() }; }
	MutableIterator end() { return { m_Components.data() + m_Count, m_Dense.data() + m_Count }; }

	ConstIterator begin() const { return { m_Components.data(), m_Dense.data() }; }
	ConstIterator end() const { return { m_Components.data() + m_Count, m_Dense.data() + m_Count }; }
private:
	std::array<Component, N> m_Components{};
	std::array<Entity, N> m_Dense{};
	std::array<size_t, N> m_Sparse;

	size_t m_Count = 0;
};

template <typename Component, size_t N>
inline ComponentArray<Component, N>::ComponentArray()
{
	m_Sparse.fill(N);
}

template <typename Component, size_t N>
inline Component& ComponentArray<Component, N>::Insert(Entity entity, Component&& component)
{
	assert(entity < N && m_Count < N);
	m_Dense[m_Count] = entity;
	m_Components[m_Count] = std::move(component);
	m_Sparse[entity] = m_Count;
	m_Count++;
	return m_Components[m_Count - 1];
}

template <typename Component, size_t N>
inline Component& ComponentArray<Component, N>::Get(Entity entity)
{
	assert(Contains(entity));
	return m_Components[m_Sparse[entity]];
}

template <typename Component, size_t N>
inline const Component& ComponentArray<Component, N>::Get(Entity entity) const
{
	assert(Contains(entity));
	return m_Components[m_Sparse[entity]];
}

template <typename Component, size_t N>
inline Component* ComponentArray<Component, N>::OptionalGet(Entity entity)
{
	if (!Contains(entity)) return nullptr;
	return &m_Components[m_Sparse[entity]];
}

template <typename Component, size_t N>
inline const Component* ComponentArray<Component, N>::OptionalGet(Entity entity) const
{
	if (!Contains(entity)) return nullptr;
	return &m_Components[m_Sparse[entity]];
}

template <typename Component, size_t N>
inline void ComponentArray<Component, N>::Remove(Entity entity)
{
	assert(entity < N);
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

template <typename Component, size_t N>
inline bool ComponentArray<Component, N>::Contains(Entity entity) const
{
	assert(entity < N);
	const size_t i = m_Sparse[entity];
	return (i < m_Count && m_Dense[i] == entity);
}
