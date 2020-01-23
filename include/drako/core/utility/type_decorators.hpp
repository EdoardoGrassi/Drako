#pragma once
#ifndef DRAKO_TYPE_DECORATORS_HPP
#define DRAKO_TYPE_DECORATORS_HPP

#include <type_traits>

namespace drako
{
    // CLASS TEMPLATE
    // Wrapper that manages a possibly null value.
    template <typename T, T NullValue>
    class nullable
    {
    public:

        explicit nullable() noexcept;
        ~nullable() noexcept(std::is_nothrow_destructible_v<T>);

        constexpr bool has_value() noexcept
        {
            return _value != NullValue;
        }
        
    private:

        T _value;
    };

} // namespace drako

#endif // !DRAKO_TYPE_DECORATORS_HPP
