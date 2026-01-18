#include "PoolAllocator.h"
#include "../Platform/PlatformMemory.h"
#include <algorithm>

PoolAllocator::PoolAllocator(size_t objectSize, size_t maxObjects)
{
    AllocPool(objectSize, maxObjects);
}

PoolAllocator::PoolAllocator(PoolAllocator&& other)
    : m_Mem{other.m_Mem}, m_FreeHead{other.m_FreeHead},
      m_MaxObjects{other.m_MaxObjects}, m_ObjectSize{other.m_ObjectSize}
{
    other.m_Mem = nullptr;
    other.m_FreeHead = nullptr;
    other.m_MaxObjects = 0;
    other.m_ObjectSize = 0;
}

PoolAllocator& PoolAllocator::operator=(PoolAllocator&& other)
{
    if (&other == this)
        return *this;
    FreePool();

    m_Mem = other.m_Mem;
    m_FreeHead = other.m_FreeHead;
    m_MaxObjects = other.m_MaxObjects;
    m_ObjectSize = other.m_ObjectSize;

    other.m_Mem = nullptr;
    other.m_FreeHead = nullptr;
    other.m_MaxObjects = 0;
    other.m_ObjectSize = 0;
    return *this;
}

void* PoolAllocator::AllocRaw()
{
    if (m_FreeHead == nullptr)
        return nullptr;

    void* const ptr = reinterpret_cast<void*>(m_FreeHead);
    m_FreeHead = m_FreeHead->next;
    return ptr;
}

void PoolAllocator::DeallocRaw(void* ptr)
{
    PoolNode* node = reinterpret_cast<PoolNode*>(ptr);
    node->next = m_FreeHead;
    m_FreeHead = node;
}

void PoolAllocator::AllocPool(size_t objectSize, size_t maxObjects)
{
    m_MaxObjects = maxObjects;
    m_ObjectSize = std::max(sizeof(PoolNode), objectSize);
    const size_t allocSize = m_MaxObjects * objectSize;

    m_Mem = Platform::MemCommitAndReserve(nullptr, allocSize);
    ResetPool();
}

void PoolAllocator::ResetPool()
{
    if (m_Mem == nullptr)
        return;

    std::byte* const memBytes = static_cast<std::byte*>(m_Mem);
    const size_t lastObjectOffset = m_ObjectSize * (m_MaxObjects - 1);

    for (size_t offset = 0; offset < lastObjectOffset; offset += m_ObjectSize)
    {
        PoolNode* const node = reinterpret_cast<PoolNode*>(memBytes + offset);
        PoolNode* const next =
            reinterpret_cast<PoolNode*>(memBytes + offset + m_ObjectSize);
        node->next = next;
    }

    PoolNode* const last =
        reinterpret_cast<PoolNode*>(memBytes + lastObjectOffset);
    last->next = nullptr;
    m_FreeHead = reinterpret_cast<PoolNode*>(memBytes);
}

void PoolAllocator::FreePool()
{
    if (m_Mem != nullptr)
        Platform::MemFree(m_Mem, m_ObjectSize * m_MaxObjects);
    m_Mem = nullptr;
    m_FreeHead = nullptr;
    m_MaxObjects = 0;
}
