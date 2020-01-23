#pragma once

#include "drako/core/builtins/intrinsics.hpp"

#include <cassert>
#include <limits>

using namespace drako;

template <typename T>
inline void validate_intrinsics()
{
    const auto MIN = std::numeric_limits<T>::min();
    const auto MAX = std::numeric_limits<T>::max();

    assert(count_zeros(MIN) == (sizeof(T) * 8));
    assert(count_zeros(MAX) == 0);
    assert(count_zeros(T{ 0b0001111 }) == 4);

    assert(count_ones(MIN) == 0);
    assert(count_ones(MAX) == (sizeof(T) * 8));

    assert(count_leading_zeros(MAX) == 32);
    assert(count_leading_zeros(MIN) == 0);
    for (auto i = 0; i <= (sizeof(T) * 8); ++i)
    {
        assert(count_leading_zeros(MAX >> i) == i);
    }

    assert(count_leading_ones(MIN) == 0);
    assert(count_leading_ones(MAX) == (sizeof(T) * 8));
    for (auto i = 0; i <= 32; ++i)
    {
        assert(count_leading_ones((~MAX) >> i)) == i);
    }
}

int main()
{
    /*
    const auto U32_MIN = std::numeric_limits<uint32_t>::min();
    const auto U32_MAX = std::numeric_limits<uint32_t>::max();

    assert(count_zeros(U32_MIN) == 32);
    assert(count_zeros(U32_MAX) == 0);
    assert(count_zeros(uint32_t{ 0b0001111 }) == 4);

    assert(count_ones(U32_MIN) == 0);
    assert(count_ones(U32_MAX) == 32);

    assert(count_leading_zeros(U32_MAX) == 32);
    assert(count_leading_zeros(std::numeric_limits<uint32_t>::max()) == 0);
    for (auto i = 0; i <= 32; ++i)
    {
        assert(count_leading_zeros(uint32_t{ (std::numeric_limits<uint32_t>::max() >> i) }) == i);
    }

    assert(count_leading_ones(std::numeric_limits<uint32_t>::min()) == 0);
    assert(count_leading_ones(std::numeric_limits<uint32_t>::max()) == 32);
    for (auto i = 0; i <= 32; ++i)
    {
        assert(count_leading_ones(uint32_t{ ~(std::numeric_limits<uint32_t>::max() >> i) }) == i);
    }
    */

    validate_intrinsics<uint32_t>();
    validate_intrinsics<uint64_t>();

    return EXIT_SUCCESS;
}