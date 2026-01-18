#pragma once

#include <cstddef>

namespace Platform
{
void* MemReserve(void* addr, size_t numBytes);

void* MemCommitAndReserve(void* addr, size_t numBytes);

void* MemCommitReserved(void* addr, size_t numBytes);

bool MemFree(void* addr, size_t numBytes);
} // namespace Platform
