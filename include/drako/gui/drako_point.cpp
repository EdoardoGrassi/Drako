#include "drako_point.hpp"

namespace Drako::gui
{
    DkPoint& DkPoint::operator+=(const DkPoint rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    DkPoint& DkPoint::operator-=(const DkPoint rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    DkPoint& DkPoint::operator*=(float value)
    {
        x *= value;
        y *= value;
        return *this;
    }

    DkPoint& DkPoint::operator/=(float value)
    {
        x /= value;
        y /= value;
        return *this;
    }

    constexpr DkPoint operator+(const DkPoint lhs, const DkPoint rhs)
    {
        return DkPoint(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    constexpr DkPoint operator-(const DkPoint lhs, const DkPoint rhs)
    {
        return DkPoint(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    constexpr DkPoint operator*(const DkPoint lhs, float value)
    {
        return DkPoint(lhs.x * value, lhs.y * value);
    }

    constexpr DkPoint operator*(float value, const DkPoint rhs)
    {
        return rhs * value;
    }

    constexpr DkPoint operator/(const DkPoint lhs, float value)
    {
        return DkPoint(lhs.x / value, lhs.y / value);
    }

    constexpr DkPoint operator/(float value, const DkPoint rhs)
    {
        return rhs / value;
    }

    constexpr bool operator==(const DkPoint lhs, const DkPoint rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    constexpr bool operator!=(const DkPoint lhs, const DkPoint rhs)
    {
        return !(lhs == rhs);
    }

    constexpr std::ostream& operator<<(std::ostream& os, const DkPoint rhs)
    {
        os << rhs.x << rhs.y;
        return os;
    }

    constexpr std::istream& operator>>(std::istream& is, DkPoint& rhs)
    {
        is >> rhs.x >> rhs.y;
        return is;
    }
}
