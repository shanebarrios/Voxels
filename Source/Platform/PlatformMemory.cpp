#include "PlatformMemory.h"
#include <cassert>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Platform
{
void* MemReserve(void* addr, size_t numBytes)
{
    return VirtualAlloc(addr, numBytes, MEM_RESERVE, PAGE_READWRITE);
}

void* MemCommitAndReserve(void* addr, size_t numBytes)
{
    return VirtualAlloc(addr, numBytes, MEM_COMMIT | MEM_RESERVE,
                        PAGE_READWRITE);
}

void* MemCommitReserved(void* addr, size_t numBytes)
{
    return VirtualAlloc(addr, numBytes, MEM_COMMIT, PAGE_READWRITE);
}

bool MemFree(void* addr, size_t numBytes)
{
    return VirtualFree(addr, 0, MEM_RELEASE);
}

size_t GetPageSize()
{
    static size_t pageSize = 0;
    if (pageSize != 0)
        return pageSize;
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    pageSize = si.dwPageSize;
    return pageSize;
}
} // namespace Platform

#elif defined(__APPLE__) || defined(__linux__)

#include <sys/mman.h>
#include <unistd.h>

namespace Platform
{
void* MemReserve(void* addr, size_t numBytes)
{
    void* ptr =
        mmap(addr, numBytes, PROT_NONE,
             MAP_PRIVATE | MAP_ANONYMOUS | (addr ? MAP_FIXED : 0), -1, 0);
    if (ptr == MAP_FAILED)
        return nullptr;
    return ptr;
}

void* MemCommitAndReserve(void* addr, size_t numBytes)
{
    void* ptr =
        mmap(addr, numBytes, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS | (addr ? MAP_FIXED : 0), -1, 0);
    if (ptr == MAP_FAILED)
        return nullptr;
    return ptr;
}

void* MemCommitReserved(void* addr, size_t numBytes)
{
    if (mprotect(addr, numBytes, PROT_READ | PROT_WRITE) != 0)
        return nullptr;
    return addr;
}

bool MemFree(void* addr, size_t numBytes)
{
    return munmap(addr, numBytes) == 0;
}

size_t GetPageSize()
{
    static size_t pageSize = 0;
    if (pageSize != 0)
        return pageSize;

    pageSize = sysconf(_SC_PAGESIZE);
    return pageSize;
}
} // namespace Platform

#else
#error "Unsupported OS"

#endif
