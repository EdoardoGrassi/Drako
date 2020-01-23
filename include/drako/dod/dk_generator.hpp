#pragma once
#ifndef DRAKO_GENERATOR_HPP
#define DRAKO_GENERATOR_HPP

#include <type_traits>
#include <vector>

#include "core/dk_compiler_spec.hpp"

#include "dod/dk_sorting.hpp"

namespace drako::dod
{
    template <typename Integral, typename Allocator>
    using IndexTable = std::vector<Integral, Allocator>;

    template <typename Integral, typename Allocator>
    using RankTable = std::vector<Integral, Allocator>;


    // FUNCTION TEMPLATE
    // Sorts the elements in range [first, last) and generates an index table for successive lookups.
    template <typename Integral, typename Iter, typename Allocator>
    DRAKO_NODISCARD inline constexpr IndexTable<Integral, Allocator>
        generate_index_table(Iter first, Iter last) noexcept
    {
        static_assert(std::is_integral_v<Integral>,
                      "Type" DRAKO_STRINGIZE(Integral) " must be an integral type");

        using index_table_type = IndexTable<Integral, Allocator>;

        DRAKO_ASSERT(first != last);

        // Initialize index table
        index_table_type index(std::distance(first, last));
        for (auto i = 0; i < index.size(); i++)
        {
            index[i] = i;
        }
        // Sort both ranges in parallel
        multi_sort(first, last, index.begin(), index.end());

        return std::move(index);
    }

    // FUNCTION TEMPLATE
    // Sorts the elements in range [first, last) and generates a rank table for successive lookups.
    template <typename Integral, typename Iter, typename Allocator>
    DRAKO_NODISCARD inline constexpr RankTable<Integral, Allocator>
        generate_rank_table(Iter first, Iter last) noexcept
    {
        static_assert(std::is_integral_v<Integral>,
                      "Type" DRAKO_STRINGIZE(Integral) " must be an integral type");

        using rank_table_type = RankTable<Integral, Allocator>;

        DRAKO_ASSERT(first != last);

        // Initialize rank table
        rank_table_type ranker(std::distance(first, last));
        for (auto i = 0; i < ranker.size(); i++)
        {
            ranker[i] = i;
        }
        // Sort both ranges in parallel
        multi_sort(first, last, ranker.begin(), ranker.end());

        return std::move(ranker);
    }
}

#endif // !DRAKO_GENERATOR_HPP
