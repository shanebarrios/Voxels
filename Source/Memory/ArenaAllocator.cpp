#include "ArenaAllocator.h"
#include "Platform/PlatformMemory.h"
#include <cassert>
#include <stddef.h>

// Credit Jason Gregory's Game Engine Architecture
static constexpr uintptr_t AlignAddress(uintptr_t addr, size_t align)
{
    const size_t mask = align - 1;
    assert((align & mask) == 0);
    return (addr + mask) & ~mask;
}

template <typename T>
static constexpr T* AlignPtr(T* ptr, size_t align)
{
    return reinterpret_cast<T*>(
        AlignAddress(reinterpret_cast<uintptr_t>(ptr), align));
}

static uintptr_t RoundPageUp(uintptr_t addr)
{
    return AlignAddress(addr, Platform::GetPageSize());
}

ArenaAllocator::ArenaAllocator(size_t committed, size_t reserved)
{
    Init(committed, reserved);
}

ArenaAllocator::~ArenaAllocator()
{
    Free();
}

ArenaAllocator::ArenaAllocator(ArenaAllocator&& other)
    : m_Base{other.m_Base}, m_Offset{other.m_Offset},
      m_Committed{other.m_Committed}, m_Reserved{other.m_Reserved}

{
    other.m_Base = nullptr;
    other.m_Offset = 0;
    other.m_Committed = 0;
    other.m_Reserved = 0;
}

ArenaAllocator& ArenaAllocator::operator=(ArenaAllocator&& other)
{
    if (&other == this)
        return *this;

    Free();
    m_Base = other.m_Base;
    m_Reserved = other.m_Reserved;
    m_Committed = other.m_Committed;
    m_Offset = other.m_Offset;

    other.m_Base = nullptr;
    other.m_Reserved = 0;
    other.m_Committed = 0;
    other.m_Offset = 0;

    return *this;
}

void* ArenaAllocator::AllocBytes(size_t numBytes, size_t align)
{
    const size_t alignedOff = AlignAddress(m_Offset, align);
    const size_t offsetNew = alignedOff + numBytes;

    assert(offsetNew <= m_Reserved);

    if (offsetNew >= m_Committed)
    {
        const uintptr_t commitBegin =
            reinterpret_cast<uintptr_t>(m_Base) + m_Committed;
        const size_t commitSize = RoundPageUp(offsetNew - commitBegin);
        Platform::MemCommitReserved(reinterpret_cast<void*>(commitBegin),
                                    commitSize);
        m_Committed += commitSize;
    }
    void* const ptr = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(m_Base) + alignedOff);
    m_Offset = offsetNew;
    return ptr;
}

void ArenaAllocator::RestoreMarker(Marker marker)
{
    assert(marker <= m_Offset);
    m_Offset = marker;
}

void ArenaAllocator::Clear()
{
    m_Offset = 0;
}

void ArenaAllocator::Free()
{
    if (m_Base != nullptr)
        Platform::MemFree(m_Base, m_Reserved);
}

void ArenaAllocator::Init(size_t committed, size_t reserved)
{
    if (reserved == 0)
        reserved = committed;
    assert(committed <= reserved);
    assert(reserved > 0);
    m_Committed = committed;
    m_Reserved = reserved;

    m_Base = Platform::MemReserve(nullptr, reserved);
    if (m_Committed > 0)
    {
        Platform::MemCommitReserved(m_Base, m_Committed);
    }

    m_Offset = 0;
}
