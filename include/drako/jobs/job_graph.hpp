#pragma once
#ifndef DRAKO_JOB_GRAPH_HPP
#define DRAKO_JOB_GRAPH_HPP

#include <tuple>
#include <type_traits>

namespace drako
{
    // using task_input_binding = std::tuple<task, size_t>;

    template<typename ...Args>
    using task_input = std::tuple<Args...>;

    template<typename ...Args>
    using task_output = std::tuple<Args...>;

    template<typename ...Args>
    class task_desc_impl
    {

    };
    
    template<typename Function>
    using task_desc = task_desc_impl<task_input<>, task_output<std::result_of_t<Function>>>;


    template<typename Function>
    using task_input_binding = std::tuple<task_desc<Function>, size_t>


    class task_graph
    {
    public:


        template<typename ...Args, typename Other>
        explicit task_graph(task_desc<Args...> task, Other tasks) noexcept
            : task_graph(tasks)
        {
            // TODO: impl
            constexpr if (sizeof...(Args) == 0)
            {

            }
        }

        template<typename ...Args>
        explicit task_graph(task_desc<Args...> task) noexcept
        {
            // TODO: impl
        }

    private:
    };
}

#endif // !DRAKO_JOB_GRAPH_HPP
