#pragma once

#include <utility>

struct PoolNode
{
    PoolNode* next;
};

class PoolAllocator
{
  public:
    PoolAllocator() = default;
    explicit PoolAllocator(size_t objectSize, size_t maxObjects);

    ~PoolAllocator() { FreePool(); }

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator(PoolAllocator&&);

    PoolAllocator& operator=(const PoolAllocator&) = delete;
    PoolAllocator& operator=(PoolAllocator&&);

    template <typename T>
    T* Alloc()
    {
        return reinterpret_cast<T*>(AllocRaw());
    }
    template <typename T>
    void Dealloc(T* ptr)
    {
        DeallocRaw(reinterpret_cast<void*>(ptr));
    }

    void* AllocRaw();
    void DeallocRaw(void* ptr);

    template <typename T, typename... Args>
    T* New(Args&&... args);

    template <typename T>
    void Delete(T* ptr)
    {
        ptr->~T();
        Dealloc(ptr);
    }

    void AllocPool(size_t objectSize, size_t numObjects);
    void ResetPool();

    void FreePool();

  private:
    void* m_Mem = nullptr;
    PoolNode* m_FreeHead = nullptr;
    size_t m_MaxObjects = 0;
    size_t m_ObjectSize = 0;
};

template <typename T, typename... Args>
T* PoolAllocator::New(Args&&... args)
{
    if (m_FreeHead == nullptr)
        return nullptr;

    PoolNode* const newHead = m_FreeHead->next;
    T* const ptr = new (m_FreeHead) T(std::forward<Args>(args)...);
    m_FreeHead = newHead;
    return ptr;
}
