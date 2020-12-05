#pragma once
#ifndef DRAKO_FIXED_VECTOR_HPP
#define DRAKO_FIXED_VECTOR_HPP

#include <cassert>
#include <memory>

namespace drako
{
    template <typename T, typename Al = std::allocator<T>>
    class FixedVector
    {
    public:
    private:
        Al          _al;
        T*          _data;
        std::size_t _size;
    };

} // namespace drako

#endif // !DRAKO_FIXED_VECTOR_HPP