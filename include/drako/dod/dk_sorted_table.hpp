#ifndef DRAKO_SORTED_TABLE_HPP
#define DRAKO_SORTED_TABLE_HPP

#include <cstddef>

#include "core/dk_vector.hpp"

namespace drako::dod
{
    // TODO: add sorting policy

    // Data table that keeps records sorted by key using insertion/deletion sort.
    template <typename TKey, typename TVal>
    class SortedTable final
    {
        static_assert(std::is_integral_v<TKey>, "Key type must be an integer");

    public:

        explicit SortedTable();
        explicit SortedTable(size_t const size);
        ~SortedTable() noexcept = default;

        SortedTable(SortedTable const&);
        SortedTable& operator=(SortedTable const&);

        SortedTable(SortedTable&&) noexcept;
        SortedTable& operator=(SortedTable&&) noexcept;

        void insert(const TKey key, const TVal value);
        void insert(TKey const* pKeys, TVal const* pValues, size_t const count);

        void remove(const TKey key) noexcept;

        TVal search(const TKey key) const noexcept;

    private:
        
        drako::FastVector<TKey> _keys;
        drako::FastVector<TVal> _values;
    };

    // TODO: end implementation

    template <typename TKey, typename TVal, typename TSortingOrderPolicy>
    class ValueSortedTable final
    {};

    struct IncrementalSortingPolicy {};

    struct DecrementalSortingPolicy {};
}

#endif // !DRAKO_SORTED_TABLE_HPP

