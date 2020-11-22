#include "drako/core/intrinsics.hpp"

#include <array>
#include <bit>
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
        assert(count_leading_ones((~MAX) >> i) == i);
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

    //validate_intrinsics<uint32_t>();
    //validate_intrinsics<uint64_t>();

    const std::uint16_t u16 = 0xaabb;
    const std::uint32_t u32 = 0xaabbccdd;
    const std::uint64_t u64 = 0xaabbccdd00112233;

    assert(byte_swap(u16) == 0xbbaa);
    assert(byte_swap(u32) == 0xddccbbaa);
    assert(byte_swap(u64) == 0x33221100ddccbbaa);

    assert(byte_swap(byte_swap(u16)) == u16);
    assert(byte_swap(byte_swap(u32)) == u32);
    assert(byte_swap(byte_swap(u64)) == u64);

    return EXIT_SUCCESS;
}