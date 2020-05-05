#pragma once
#ifndef DRAKO_INPUT_SYSTEM_UTILS_HPP_
#define DRAKO_INPUT_SYSTEM_UTILS_HPP_

#include <limits>
#include <type_traits>

namespace drako::engine
{
    // Normalize raw value to fit in range [0, +1]
    template <typename UnsignedInt,
        std::enable_if_t<std::is_integral_v<UnsignedInt> && std::is_unsigned_v<UnsignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_unidirectional_axis(UnsignedInt _raw) noexcept
    {
        static_assert(std::is_integral_v<UnsignedInt>);
        static_assert(std::is_unsigned_v<UnsignedInt>);

        return static_cast<float>(_raw) / std::numeric_limits<UnsignedInt>::max();
    }

    // Normalize raw value from [min, max] to fit in range [0, +1].
    template <typename UnsignedInt,
        std::enable_if_t<std::is_integral_v<UnsignedInt> && std::is_unsigned_v<UnsignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_unidirectional_axis(UnsignedInt _raw, UnsignedInt _min, UnsignedInt _max) noexcept
    {
        static_assert(std::is_integral_v<UnsignedInt>);
        static_assert(std::is_unsigned_v<UnsignedInt>);

        return static_cast<float>(_raw - _min) / (_max - _min);
    }


    // Normalize raw value to fit in range [0, +1]
    template <typename SignedInt,
        std::enable_if_t<std::is_integral_v<SignedInt> && std::is_signed_v<SignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_unidirectional_axis(SignedInt _raw) noexcept
    {
        static_assert(std::is_integral_v<SignedInt>);
        static_assert(std::is_signed_v<SignedInt>);

        // from [min, max] to [-0.5, 0.5] then to [0, +1]
        return static_cast<float>(_raw) / (2 * std::numeric_limits<SignedInt>::max()) + 0.5f;
    }

    // Normalize raw value from [min, max] to fit in range [0, +1].
    template <typename SignedInt,
        std::enable_if_t<std::is_integral_v<SignedInt> && std::is_signed_v<SignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_unidirectional_axis(SignedInt _raw, SignedInt _min, SignedInt _max) noexcept
    {
        static_assert(std::is_integral_v<SignedInt>);
        static_assert(std::is_signed_v<SignedInt>);

        // from [min, max] to [-0.5, 0.5] then to [0, +1]
    }


    // Normalize raw value to fit in range [-1, +1]
    template <typename UnsignedInt,
        std::enable_if_t<std::is_integral_v<UnsignedInt> && std::is_unsigned_v<UnsignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_bidirectional_axis(UnsignedInt _raw) noexcept
    {
        static_assert(std::is_integral_v<UnsignedInt>);
        static_assert(std::is_unsigned_v<UnsignedInt>);

        // from [0, MAX] to [0, +2] then to [-1, +1]
        return static_cast<float>(_raw) / (std::numeric_limits<UnsignedInt>::max() / 2) - 1.f;
    }

    // Normalize raw value from [min, max] to fit in range [-1, +1].
    template <typename UnsignedInt,
        std::enable_if_t<std::is_integral_v<UnsignedInt> && std::is_unsigned_v<UnsignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_bidirectional_axis(UnsignedInt _raw, UnsignedInt _min, UnsignedInt _max) noexcept
    {
        static_assert(std::is_integral_v<UnsignedInt>);
        static_assert(std::is_unsigned_v<UnsignedInt>);

        // from [min, max] to [0, +2] then to [-1, +1]
        return static_cast<float>(_raw - _min) / ((_max - _min) / 2) - 1.f;
    }

    // Normalize raw value to fit in range [-1, +1]
    template <typename SignedInt,
        std::enable_if_t<std::is_integral_v<SignedInt> && std::is_signed_v<SignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_bidirectional_axis(SignedInt _raw) noexcept
    {
        static_assert(std::is_integral_v<SignedInt>);
        static_assert(std::is_signed_v<SignedInt>);

        // from [0, MAX] to [0, +2] then to [-1, +1]
        return static_cast<float>(_raw) / (std::numeric_limits<SignedInt>::max() / 2) - 1.f;
    }

    // Normalize raw value from [min, max] to fit in range [-1, +1].
    template <typename SignedInt,
        std::enable_if_t<std::is_integral_v<SignedInt> && std::is_signed_v<SignedInt>, int> = 0>
    [[nodiscard]] inline constexpr float
    normalize_bidirectional_axis(SignedInt _raw, SignedInt _min, SignedInt _max) noexcept
    {
        static_assert(std::is_integral_v<SignedInt>);
        static_assert(std::is_signed_v<SignedInt>);

        // from [min, max] to [0, +2] then to [-1, +1]
        return static_cast<float>(_raw - _min) / ((_max - _min) / 2) - 1.f;
    }



    [[nodiscard]] inline float analog_stick_deadzone() noexcept;


    // Filter source value within a deadzone.
    [[nodiscard]] inline constexpr float
    deadzone(float raw, float min, float max) noexcept
    {
        raw = (raw < min) ? 0.f : raw;
        raw = (raw > max) ? 1.f : raw;
        return raw;
    }

} // namespace drako::engine

#endif // !DRAKO_INPUT_SYSTEM_UTILS_HPP_