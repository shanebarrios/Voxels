#pragma once

#include <utility>

using Marker = size_t;

class ArenaAllocator
{
  public:
    ArenaAllocator() = default;
    ArenaAllocator(size_t committed, size_t reserved);

    ~ArenaAllocator();

    ArenaAllocator(const ArenaAllocator&) = delete;
    ArenaAllocator(ArenaAllocator&&);

    ArenaAllocator& operator=(const ArenaAllocator&) = delete;
    ArenaAllocator& operator=(ArenaAllocator&&);

    template <typename T>
    T* Alloc();

    void* AllocBytes(size_t numBytes, size_t align);

    template <typename T, typename... Args>
    T* New(Args&&... args);

    Marker GetMarker() const { return m_Offset; }

    void RestoreMarker(Marker marker);

    void Clear();

    void Free();

    void Init(size_t committed, size_t reserved);

  private:
    void* m_Base;
    size_t m_Offset;
    size_t m_Committed;
    size_t m_Reserved;
};

template <typename T>
T* ArenaAllocator::Alloc()
{
    return reinterpret_cast<T*>(AllocBytes(sizeof(T), alignof(T)));
}

template <typename T, typename... Args>
T* ArenaAllocator::New(Args&&... args)
{
    return new (AllocBytes(sizeof(T), alignof(T)))
        T(std::forward<Args>(args)...);
}
