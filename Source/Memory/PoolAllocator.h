#pragma once

#include "../Platform/PlatformMemory.h"
#include <algorithm>

struct PoolNode
{
    PoolNode* next;
};

template <typename T>
class PoolAllocator
{
  public:
    PoolAllocator() = default;
    explicit PoolAllocator(size_t maxObjects);

    T* Alloc();
    void Dealloc(T* ptr);

    template <typename... Args>
    T* New(Args&&... args);
    void Delete(T* ptr);

    void AllocPool(size_t numObjects);
    void ResetPool();

    void FreePool();

  private:
    void* m_Mem = nullptr;
    PoolNode* m_FreeHead = nullptr;
    size_t m_MaxObjects = 0;
};

template <typename T>
inline PoolAllocator<T>::PoolAllocator(size_t maxObjects)
{
    AllocPool(maxObjects);
}

template <typename T>
inline T* PoolAllocator<T>::Alloc()
{
    if (m_FreeHead == nullptr)
        return nullptr;

    T* const ptr = reinterpret_cast<T*>(m_FreeHead);
    m_FreeHead = m_FreeHead->next;
    return ptr;
}

template <typename T>
inline void PoolAllocator<T>::Dealloc(T* ptr)
{
    PoolNode* node = reinterpret_cast<PoolNode*>(ptr);
    node->next = m_FreeHead;
    m_FreeHead = node;
}

template <typename T>
template <typename... Args>
inline T* PoolAllocator<T>::New(Args&&... args)
{
    if (m_FreeHead == nullptr)
        return nullptr;

    PoolNode* const newHead = m_FreeHead->next;
    T* const ptr = new (m_FreeHead) T(std::forward<Args>(args)...);
    m_FreeHead = newHead;
    return ptr;
}

template <typename T>
inline void PoolAllocator<T>::Delete(T* ptr)
{
    ptr->~T();
    Dealloc(ptr);
}

template <typename T>
inline void PoolAllocator<T>::AllocPool(size_t maxObjects)
{
    constexpr size_t objectSize = std::max(sizeof(PoolNode*), sizeof(T));
    const size_t allocSize = maxObjects * objectSize;
    m_MaxObjects = maxObjects;

    m_Mem = Platform::MemCommitAndReserve(nullptr, allocSize);
    ResetPool();
}

template <typename T>
inline void PoolAllocator<T>::ResetPool()
{
    PoolNode* const nodes = reinterpret_cast<PoolNode*>(m_Mem);

    for (size_t i = 0; i < m_MaxObjects - 1; i++)
    {
        nodes[i].next = &nodes[i + 1];
    }
    nodes[m_MaxObjects - 1].next = nullptr;

    m_FreeHead = nodes;
}

template <typename T>
inline void PoolAllocator<T>::FreePool()
{
    constexpr size_t objectSize = std::max(sizeof(PoolNode*), sizeof(T));
    Platform::MemFree(m_Mem, objectSize);
    m_Mem = nullptr;
    m_FreeHead = nullptr;
    m_MaxObjects = 0;
}
