#pragma once
#ifndef INPUT_UTILS_HPP
#define INPUT_UTILS_HPP

#include <limits>
#include <type_traits>

namespace input
{
    // Normalize raw value to fit in range [0, +1]
    template <typename UInt> /* clang-format off */
    requires std::is_integral_v<UInt> && std::is_unsigned_v<UInt>
    [[nodiscard]] inline constexpr float
    normalize_unsigned_axis(UInt raw) noexcept /* clang-format on */
    {
        return static_cast<float>(raw) / std::numeric_limits<UInt>::max();
    }

    static_assert(normalize_unsigned_axis(std::numeric_limits<uint16_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<uint16_t>::max()) == 1.f);

    static_assert(normalize_unsigned_axis(std::numeric_limits<uint32_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<uint32_t>::max()) == 1.f);

    static_assert(normalize_unsigned_axis(std::numeric_limits<uint64_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<uint64_t>::max()) == 1.f);


    // Normalize raw value from [min, max] to fit in range [0, +1].
    template <typename UInt> /* clang-format off */
    requires std::is_integral_v<UInt>&& std::is_unsigned_v<UInt>
    [[nodiscard]] inline constexpr float
    normalize_unsigned_axis(UInt raw, UInt min, UInt max) noexcept /* clang-format on */
    {
        //return static_cast<float>(raw - min) / (max - min);
    }


    // Normalize raw value to fit in range [0, +1]
    template <typename SInt> /* clang-format off */
    requires std::is_integral_v<SInt> && std::is_signed_v<SInt>
    [[nodiscard]] inline constexpr float
    normalize_unsigned_axis(SInt raw) noexcept /* clang-format on */
    {
        // from [min, max] to [-0.5, 0.5] then to [0, +1]
        const auto f = static_cast<float>(raw);
        return f < 0 ? -f / std::numeric_limits<SInt>::min() / 2 + 0.5f
                     : f / std::numeric_limits<SInt>::max() / 2 + 0.5f;
    }
    static_assert(normalize_unsigned_axis(std::numeric_limits<int16_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(int16_t{ 0 }) == 0.5f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<int16_t>::max()) == 1.f);

    static_assert(normalize_unsigned_axis(std::numeric_limits<int32_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(int32_t{ 0 }) == 0.5f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<int32_t>::max()) == 1.f);

    static_assert(normalize_unsigned_axis(std::numeric_limits<int64_t>::min()) == 0.f);
    static_assert(normalize_unsigned_axis(int64_t{ 0 }) == 0.5f);
    static_assert(normalize_unsigned_axis(std::numeric_limits<int64_t>::max()) == 1.f);


    // Normalize raw value from [min, max] to fit in range [0, +1].
    template <typename SInt> /* clang-format off */
    requires std::is_integral_v<SInt> && std::is_signed_v<SInt>
    [[nodiscard]] inline constexpr float
    normalize_unsigned_axis(SInt raw, SInt min, SInt max) noexcept /* clang-format on */
    {
        // from [min, max] to [-0.5, 0.5] then to [0, +1]
    }


    // Normalize raw value to fit in range [-1, +1]
    template <typename UInt> /* clang-format off */
    requires std::is_integral_v<UInt> && std::is_unsigned_v<UInt>
    [[nodiscard]] inline constexpr float
    normalize_signed_axis(UInt raw) noexcept /* clang-format on */
    {
        // from [0, MAX] to [0, +2] then to [-1, +1]
        return static_cast<float>(raw) / std::numeric_limits<UInt>::max() * 2 - 1.f;
    }

    static_assert(normalize_signed_axis(std::numeric_limits<uint16_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(std::numeric_limits<uint16_t>::max()) == 1.f);

    static_assert(normalize_signed_axis(std::numeric_limits<uint32_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(std::numeric_limits<uint32_t>::max()) == 1.f);

    static_assert(normalize_signed_axis(std::numeric_limits<uint64_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(std::numeric_limits<uint64_t>::max()) == 1.f);


    // Normalize raw value from [min, max] to fit in range [-1, +1].
    template <typename UInt> /* clang-format off */
    requires std::is_integral_v<UInt> && std::is_unsigned_v<UInt>
    [[nodiscard]] inline constexpr float
    normalize_signed_axis(UInt raw, UInt min, UInt max) noexcept /* clang-format on */
    {
        // from [min, max] to [0, +2] then to [-1, +1]
        //return static_cast<float>(raw - min) / ((max - min) / 2) - 1.f;
    }


    // Normalize raw value to fit in range [-1, +1]
    template <typename SInt> /* clang-format off */
    requires std::is_integral_v<SInt> && std::is_signed_v<SInt>
    [[nodiscard]] inline constexpr float
    normalize_signed_axis(SInt raw) noexcept /* clang-format on */
    {
        // from [-MAX, +MAX] to [-1, +1]
        //return static_cast<float>(raw) / std::numeric_limits<SInt>::max();
        const auto f = static_cast<float>(raw);
        return f < 0 ? -f / std::numeric_limits<SInt>::min()
                     : f / std::numeric_limits<SInt>::max();
    }

    static_assert(normalize_signed_axis(std::numeric_limits<int16_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(int16_t{ 0 }) == 0.f);
    static_assert(normalize_signed_axis(std::numeric_limits<int16_t>::max()) == 1.f);

    static_assert(normalize_signed_axis(std::numeric_limits<int32_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(int32_t{ 0 }) == 0.f);
    static_assert(normalize_signed_axis(std::numeric_limits<int32_t>::max()) == 1.f);

    static_assert(normalize_signed_axis(std::numeric_limits<int64_t>::min()) == -1.f);
    static_assert(normalize_signed_axis(int64_t{ 0 }) == 0.f);
    static_assert(normalize_signed_axis(std::numeric_limits<int64_t>::max()) == 1.f);


    // Normalize raw value from [min, max] to fit in range [-1, +1].
    template <typename SInt> /* clang-format off */
    requires std::is_integral_v<SInt> && std::is_signed_v<SInt>
    [[nodiscard]] inline constexpr float
    normalize_signed_axis(SInt raw, SInt min, SInt max) noexcept /* clang-format on */
    {
        // from [min, max] to [0, +2] then to [-1, +1]
        //return static_cast<float>(raw - min) / ((max - min) / 2) - 1.f;
    }



    [[nodiscard]] inline float analog_stick_deadzone() noexcept;


    // Filter source value within a deadzone.
    [[nodiscard]] inline constexpr float deadzone(float raw, float min, float max) noexcept
    {
        raw = (raw < min) ? 0.f : raw;
        raw = (raw > max) ? 1.f : raw;
        return raw;
    }

} // namespace input

#endif // !INPUT_UTILS_HPP