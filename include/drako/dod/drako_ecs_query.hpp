#ifndef DRAKO_ECS_QUERY_HPP_
#define DRAKO_ECS_QUERY_HPP_

#include "drako_ecs_table.hpp"

namespace drako::dod
{
    struct EntityQuery
    {
        template <typename TKey, typename... Args>
        static void request_components(DataTable<TKey, Args>&..., QueryResultIterator<Args...>& out_result);
    };


    template <typename... Args>
    class QueryResultIterator
    {
    public:

        constexpr explicit QueryResultIterator();
        ~QueryResultIterator() noexcept;

    private:

        struct IteratorRecord
        {
            std::tuple<Args*...> components_ptr;
            std::size_t components_count;
        };

        std::size_t m_idx;
    };
}

#endif // !DRAKO_ECS_QUERY_HPP_
