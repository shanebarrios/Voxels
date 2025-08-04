#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string_view>
#include <memory>
#include <glm/glm.hpp>

#include "ComponentArray.h"

using ComponentID = uint32_t;

class ECS
{
public:
	ECS() = default;

	Entity CreateEntity();

	void DestroyEntity(Entity entity);

	template <typename Component, size_t MaxComponents = 8192>
	void RegisterComponent();

	template <typename Component> 
	Component& AddComponent(Entity entity, Component&& component);

	template <typename Component>
	void RemoveComponent(Entity entity);

	template <typename Component>
	Component& GetComponent(Entity entity);

	template <typename Component>
	const Component& GetComponent(Entity entity) const;

	template <typename Component>
	Component* GetOptionalComponent(Entity entity);

	template <typename Component>
	const Component* GetOptionalComponent(Entity entity) const;

	template <typename Component>
	bool HasComponent(Entity entity) const;

	template <typename Component>
	ComponentArray<Component>& GetComponentArray();

	template <typename Component>
	const ComponentArray<Component>& GetComponentArray() const;

private:
	template <typename Component>
	ComponentID GetComponentID() const;

	ComponentID GenerateComponentID() const;

private:
	Entity m_EntityCount = 0;

	std::array<std::unique_ptr<IComponentArray>, 32> m_ComponentArrays{};

	std::vector<Entity> m_FreedList{};
};

template <typename Component>
inline ComponentID ECS::GetComponentID() const
{
	static ComponentID ID = GenerateComponentID();
	return ID;
}

inline ComponentID ECS::GenerateComponentID() const
{
	static ComponentID ID = 0;
	return ID++;
}

template <typename Component>
inline ComponentArray<Component>& ECS::GetComponentArray()
{
	ComponentID id = GetComponentID<Component>();
	assert(id < 32 && m_ComponentArrays[id]);
	return *dynamic_cast<ComponentArray<Component>*>(m_ComponentArrays[id].get());
}

template <typename Component>
inline const ComponentArray<Component>& ECS::GetComponentArray() const
{
	ComponentID id = GetComponentID<Component>();
	assert(m_ComponentArrays[id]);
	return *dynamic_cast<ComponentArray<Component>*>(m_ComponentArrays[id].get());
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
		return m_EntityCount++;
	}
}

inline void ECS::DestroyEntity(Entity entity)
{
	m_FreedList.push_back(entity);

	for (ComponentID i = 0; m_ComponentArrays[i]; i++)
	{
		m_ComponentArrays[i]->Destroy(entity);
	}
}

template <typename Component, size_t MaxComponents>
inline void ECS::RegisterComponent()
{
	ComponentID id = GetComponentID<Component>();
	assert(id < 32 && !m_ComponentArrays[id]);
	m_ComponentArrays[id] = std::make_unique<ComponentArray<Component, MaxComponents>>();
}

template <typename Component>
inline Component& ECS::AddComponent(Entity entity, Component&& component)
{
	return GetComponentArray<Component>().Insert(entity, std::move(component));
}

template <typename Component>
inline void ECS::RemoveComponent(Entity entity)
{
	GetComponentArray<Component>().Remove(entity);
}

template <typename Component>
inline Component& ECS::GetComponent(Entity entity)
{
	return GetComponentArray<Component>().Get(entity);
}

template <typename Component>
inline const Component& ECS::GetComponent(Entity entity) const
{
	return GetComponentArray<Component>().Get(entity);
}

template <typename Component>
inline Component* ECS::GetOptionalComponent(Entity entity)
{
	return GetComponentArray<Component>().OptionalGet(entity);
}

template <typename Component>
const Component* ECS::GetOptionalComponent(Entity entity) const
{
	return GetComponentArray<Component>().OptionalGet(entity);
}

template <typename Component>
inline bool ECS::HasComponent(Entity entity) const
{
	return GetComponentArray<Component>().Contains(entity);
}
