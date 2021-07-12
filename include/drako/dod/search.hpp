#pragma once
#ifndef DRAKO_ALGORITHM_SEARCH_HPP
#define DRAKO_ALGORITHM_SEARCH_HPP

//
// \brief   Collection of templatized searching algorithms.
// \author  Grassi Edoardo.
//


#include <iterator>
#include <type_traits>

namespace drako
{
    // FUNCTION TEMPLATE
    // Finds the first element in range [first, last) that is lesser or equal to any other.
    template <typename Iter>
    [[nodiscard]] inline constexpr Iter
    find_first_min(Iter first, Iter last) noexcept
    {
        using iter_category = typename std::iterator_traits<Iter>::iterator_category;

        static_assert(std::is_base_of_v<std::forward_iterator_tag, iter_category>,
                      "Type " DRAKO_STRINGIZE(Iter) " must support forward iterator functionality";

        if (first == last) {
            return last; } // Range is empty

        // Linear search
        auto curr_min = first;
        while (++first != last)
        {
            if (*first < *curr_min)
            {
                curr_min = first;
            }
        }
        return curr_min;
    }

    // FUNCTION TEMPLATE
    // Finds the first element in range [first, last) that is lesser or equal to any other.
    template <typename Iter>
    [[nodiscard]] inline constexpr Iter
    find_first_max(Iter first, Iter last) noexcept
    {
        std::iterator_traits<Iter>::iterator_category;

        if (first == last)
        {
            return last;
        } // Range is empty

        // Linear search
        auto curr_max = first;
        while (++first != last)
        {
            if (*first > *curr_max)
            {
                curr_max = first;
            }
        }
        return curr_max;
    }

    // FUNCTION TEMPLATE
    // Finds the two first elements in range [first, last) that are either the minimum ot he maximum.
    template <typename Iter>
    [[nodiscard]] inline constexpr std::tuple<Iter, Iter>
    find_first_min_max(Iter first, Iter last) noexcept;


    // FUNCTION TEMPLATE
    // Finds the first element in range [first, last) that satisfies a certain condition.
    template <typename Iter, typename UnaryPredicate>
    [[nodiscard]] inline constexpr Iter
    find_first_where(Iter first, Iter last, UnaryPredicate condition) noexcept
    {
        static_assert(std::is_nothrow_invocable_r_v<bool, UnaryPredicate, typename std::iterator_traits<Iter>::value_type>,
            "Type " DRAKO_STRINGIZE(UnaryPredicate) " can't be invoked as unary predicate");

        while (first != last)
        {
            if (condition(*first))
            {
                break;
            }
            first++;
        }
        return first;
    }

    // FUNCTION TEMPLATE
    // Finds the last element in range [first, last) that satisfies a certain condition.
    template <typename Iter, typename UnaryPredicate>
    [[nodiscard]] inline constexpr Iter
    find_last_where(Iter first, Iter last, UnaryPredicate condition) noexcept;


    // FUNCTION TEMPLATE
    // Finds the element in range [first, last) that occupies the nth-position in sorted order.
    template <typename Iter>
    [[nodiscard]] inline constexpr Iter
    select_nth(Iter first, Iter last) noexcept;


    // FUNCTION TEMPLATE
    // Finds the first element in range [first, last) that is equal to searched value.
    //
    // Returns: an iterator that references the position of the element. Last if not found.
    //
    // Complexity: O(n)
    //
    template <typename Iter>
    [[nodiscard]] inline constexpr Iter
    linear_search(Iter first, Iter last, decltype(*first) value) noexcept;

    // FUNCTION TEMPLATE
    // Finds the first element in range [first, last) that is equal to searched value.
    //
    // Returns: an iterator that references the position of the element. Last if not found.
    //
    // Complexity: O(log n)
    //
    template <typename Iter>
    [[nodiscard]] inline constexpr Iter
    binary_search(Iter first, Iter last, decltype(*first) value) noexcept
    {
        if (first == last)
        {
            return last;
        } // Empty range

        while (first < last)
        {
            Iter pivot = first + std::distance(first, last) / 2;

            if (*pivot == value)
            {
                return pivot;
            }
            else if (*pivot < value)
            {
                last = --pivot;
            }
            else
            {
                first = ++pivot;
            }
        }
        return last;
    }
} // namespace drako

#endif // !DRAKO_ALGORITHM_SEARCH_HPP
