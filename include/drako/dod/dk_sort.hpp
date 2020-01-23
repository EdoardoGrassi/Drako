//
// \brief   Collection of sorting algorithms and utilities.
// \author  Grassi Edoardo.
//

#pragma once
#ifndef DRAKO_SORT_HPP
#define DRAKO_SORT_HPP

#include <iterator>
#include <vector>

#include "core/dk_compiler_spec.hpp"

#if defined (DRKAPI_DEBUG)
#include "development/dk_assertion.hpp"
#endif

namespace drako::dod
{
    // FUNCTION TEMPLATE
    // Checks if the elements in range [first, last) are sorted.
    // Returns: true if range is empty or correctly ordered, false otherwise.
    template <typename Iter, typename Comparer>
    DRAKO_NODISCARD constexpr bool
        is_sorted(Iter first, Iter last, Comparer comp) noexcept
    {
        if (first != last)
        {
            std::advance(first);
            for (; first != last; first++)
            {
                if (!comp(first, std::next(first)))
                {
                    return false;
                }
            }
        }
        return true;
    }

    template <typename Iter, typename Comparer>
    constexpr void bubble_sort(Iter first, Iter last, Comparer comp) noexcept
    {
        auto curr = first;
        auto next = first;
        while (first != last)
        {
            // TODO: end impl
        }
    }

    // FUNCTION TEMPLATE
    // Merges the elements in sorted ranges [first1, last1) and [first2, last2) in output range.
    template <typename InputIter1, typename InputIter2, typename OutputIter>
    void merge_sort(InputIter1 first1, InputIter1 last1,
                    InputIter2 first2, InputIter2 last2,
                    OutputIter output) noexcept
    {
        if (first1 == last1)
        {
            // Move range [first2, last2) to output
        }
        if (first2 == last2)
        {
            // Move range [first1, last1) to output
        }

        // Set range1 as the greater range
        if (std::distance(first1, last1) < std::distance(first2, last2))
        {
            std::swap(first1, first2);
            std::swap(last1, last2);
        }

        while ((first1 != last1) & (first2 != last2))
        {
            if (*first <= *last)
            {
                *output++ = *first++;
            }
            else
            {
                *output++ = *last++;
            }
        }
        // TODO: end impl
    }

    // FUNCTION TEMPLATE
    // Sorts the elements in range [firt, last) in ascending order.
    template <typename Iter, typename Allocator>
    void count_sort(Iter first, Iter last,
                    typename std::iterator_traits<Iter>::value_type min,
                    typename std::iterator_traits<Iter>::value_type max) noexcept
    {
        using iter_value_type = typename std::iterator_traits<Iter>::value_type;
        using iter_diff_type = typename std::iterator_traits<Iter>::difference_type;

        static_assert(std::is_integral_v<iter_value_type>,
                      "Type " DRAKO_STRINGIZE(iter_value_type) " must be an integral type");
        static_assert(std::is_base_of_v<std::random_access_iterator_tag, Iter>,
                      "Type " DRAKO_STRINGIZE(Iter) " must support random access");

        DRAKO_ASSERT(min <= max);
        DRAKO_ASSERT(first < last);

        if (first == last) { return; } // Empty range

        std::vector<iter_value_type, Allocator> counter(max - min);
        // Count occurences in input range
        for (auto it = first; it != last; it++)
        {
            counter[(*it) - min]++;
        }
        // Generate sorted output overwriting input range
        for (auto it = counter.cbegin(); it != counter.cend(); it++)
        {
            for (auto count = *it; count > 0; count--)
            {
                *first = min + it;
                first++;
            }
        }
    }

    // FUNCTION TEMPLATE
    // Least Significant Digit (LSD) stable radix sort.
    template <typename Ty>
    void stable_radix_sort(Ty* data, size_t count) noexcept;

    // FUNCTION TEMPLATE
    // Most Significant Digit (MSD) unstable radix sort.
    template <typename Ty>
    void unstable_radix_sort(Ty* data, size_t count) noexcept;


    // FUNCTION TEMPLATE
    // Sorts the elements in range [first1, last1) in ascending order,
    // while moving the corrisponding elements in range [first2, last2).
    template <typename InputIter1, typename InputIter2>
    void multi_sort(InputIter1 first1, InputIter1 last1,
                    InputIter2 first2, InputIter2 last2) noexcept;
}

#endif // !DRAKO_SORT_HPP