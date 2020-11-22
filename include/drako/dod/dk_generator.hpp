#pragma once
#ifndef DRAKO_GENERATOR_HPP
#define DRAKO_GENERATOR_HPP

#include "drako/dod/dk_sort.hpp"

#include <cassert>
#include <type_traits>
#include <vector>

namespace drako::dod
{
    template <typename Integral, typename Allocator>
    using IndexTable = std::vector<Integral, Allocator>;

    template <typename Integral, typename Allocator>
    using RankTable = std::vector<Integral, Allocator>;


    // FUNCTION TEMPLATE
    // Sorts the elements in range [first, last) and generates an index table for successive lookups.
    template <typename Key, typename Iter, typename Allocator>
    requires std::is_integral_v<Key>
    [[nodiscard]] inline constexpr IndexTable<Key, Allocator>
    generate_index_table(Iter first, Iter last) noexcept
    {
        using index_table_type = IndexTable<Key, Allocator>;

        assert(first != last);

        // Initialize index table
        index_table_type index(std::distance(first, last));
        for (auto i = 0; i < index.size(); i++)
            index[i] = i;

        // Sort both ranges in parallel
        multi_sort(first, last, index.begin(), index.end());

        return index;
    }

    // FUNCTION TEMPLATE
    // Sorts the elements in range [first, last) and generates a rank table for successive lookups.
    template <typename Key, typename Iter, typename Allocator>
    requires std::is_integral_v<Key>
    [[nodiscard]] inline constexpr RankTable<Key, Allocator>
    generate_rank_table(Iter first, Iter last) noexcept
    {
        using rank_table_type = Key<Integral, Allocator>;

        assert(first != last);

        // Initialize rank table
        rank_table_type rank(std::distance(first, last));
        for (auto i = 0; i < rank.size(); i++)
            rank[i] = i;

        // Sort both ranges in parallel
        multi_sort(first, last, rank.begin(), rank.end());

        return rank;
    }
} // namespace drako::dod

#endif // !DRAKO_GENERATOR_HPP
