#pragma once
#ifndef DRAKO_INDEX_HPP
#define DRAKO_INDEX_HPP

#include <vector>

namespace drako::dod
{
    // CLASS TEMPLATE
    // Stores the index of the sorted records from a data table.
    // The Nth cell contains the index of Nth record in sorted order.
    template <typename Integral>
    class IndexTable__ final
    {
    public:

        using value_type    = Integral;
        using size_type     = size_t;

        explicit IndexTable__(size_type size) noexcept
            : _table(size)
        {
            for (auto i = 0; i < size; i++)
            {
                _table[i] = i;
            }
        }

    private:

        std::vector<value_type> _table;
    };

    // Stores the rank of the sorted record from a data table.
    // The Nth cell contains the rank that Nth record would have in sorted order.
    template <typename T>
    class RankTable final
    {
        static_assert(std::is_integral_v<TKey>, "Integral type required");

    public:

        explicit RankTable(size_t size) noexcept;

    private:

        size_t _size;
        T _data[];
    };
}

#endif // !DRAKO_INDEX_HPP
