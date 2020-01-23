#ifndef _DRAKO_GEOMETRY_HPP_
#define _DRAKO_GEOMETRY_HPP_

#include <iostream>

namespace Drako
{
    /// <summary>Represents a point in a 2-dimensional Cartesian system.</summary>
    template <typename T>
    struct DkPoint
    {
        /// <summary>Absissa coordinate of the point.</summary>
        T x;

        /// <summary>Ordinate coordinate of the point.</summary>
        T y;

        inline constexpr DkPoint(const T x, const T y) noexcept : x(x), y(y) {}
        inline ~DkPoint() noexcept = default;

        inline constexpr DkPoint(const DkPoint& rhs) noexcept = default;                   // Support copy
        inline constexpr DkPoint& operator=(const DkPoint& rhs) noexcept = default;

        inline constexpr DkPoint(DkPoint&& rhs) noexcept = default;                        // Support move
        inline constexpr DkPoint& operator=(DkPoint&& rhs) noexcept = default;

        inline constexpr DkPoint& operator+=(const DkPoint rhs) noexcept;
        inline constexpr DkPoint& operator-=(const DkPoint rhs) noexcept;
        inline constexpr DkPoint& operator*=(const T value) noexcept;
        inline constexpr DkPoint& operator/=(const T value) noexcept;
    };

    template <typename T>
    inline constexpr DkPoint<T> operator+(const DkPoint<T> lhs, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr DkPoint<T> operator-(const DkPoint<T> lhs, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr DkPoint<T> operator*(const DkPoint<T> lhs, const T value) noexcept;

    template <typename T>
    inline constexpr DkPoint<T> operator*(const T lhs, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr DkPoint<T> operator/(const DkPoint<T> lhs, const T value) noexcept;

    template <typename T>
    inline constexpr DkPoint<T> operator/(const T value, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr bool operator==(const DkPoint<T> lhs, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr bool operator!=(const DkPoint<T> lhs, const DkPoint<T> rhs) noexcept;

    template <typename T>
    inline constexpr std::ostream& operator<<(std::ostream& os, const DkPoint<T> rhs);

    template <typename T>
    inline constexpr std::istream& operator>>(std::istream& is, DkPoint<T>& rhs);

    using DkVector2f = DkPoint<float>;

    using DkVector2d = DkPoint<double>;
}

#endif // !_DRAKO_POINT_HPP_

