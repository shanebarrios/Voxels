#pragma once

#include <bit>
#include <cstdint>

#if defined(_MSC_VER) && !defined(__clang__)
#define unreachable() __assume(false)
#else
#define unreachable() __builtin_unreachable()
#endif

inline constexpr int CHUNK_DIMENSION = 32;
inline constexpr int CHUNK_AREA = CHUNK_DIMENSION * CHUNK_DIMENSION;
inline constexpr int CHUNK_VOLUME =
    CHUNK_DIMENSION * CHUNK_DIMENSION * CHUNK_DIMENSION;

inline constexpr size_t CHUNK_DIMENSION_U = 32u;
inline constexpr size_t CHUNK_AREA_U = CHUNK_DIMENSION_U * CHUNK_DIMENSION_U;
inline constexpr size_t CHUNK_VOLUME_U =
    CHUNK_DIMENSION_U * CHUNK_DIMENSION_U * CHUNK_DIMENSION_U;
inline constexpr size_t CHUNK_COORD_MASK = CHUNK_DIMENSION_U - 1;
inline constexpr size_t CHUNK_COORD_BIT_COUNT =
    std::popcount(static_cast<size_t>(CHUNK_COORD_MASK));