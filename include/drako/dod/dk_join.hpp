#pragma once
#ifndef DRAKO_JOIN_HPP
#define DRAKO_JOIN_HPP

#include <tuple>

namespace drako::dod
{
    // FUNCTION TEMPLATE
    //
    template <typename Iter>
    std::tuple<Iter, Iter> join_intersection(Iter first1, Iter last1,
                                             Iter first2, Iter last2) noexcept
    {
        std::size_t shortest_set_size = std::min(in_lhs.size, in_rhs.size);
        auto x, y = 0;
        while (true)
        {
            while (in_lhs[x] < in_rhs[y] && x < shortest_set_size) x++;
            while (in_rhs[y] < in_lhs[x] && y < shortest_set_size) y++;
            if (in_lhs[x] == in_lhs[y])
            {
                out_result.push_back(in_lhs[x]);
                x++;
                y++;
            }
            else
            {
                break; // reached the end of the smallest set
            }
        }
    }

    // FUNCTION TEMPLATE
    //
    template <typename Iter>
    std::tuple<Iter, Iter> join_difference(Iter first1, Iter last1,
                                           Iter first2, Iter last2) noexcept;

    template <typename T>
    void join_union(const std::vector<T>& in_lhs,
                    const std::vector<T>& in_rhs,
                    std::vector<T>& out_result) noexcept
    {
        std::size_t shortest_set_size = std::min<std::size_t>(in_lhs.size, in_rhs.size);
        auto x = 0, y = 0;
        while (true)
        {
            while (in_lhs[x] < in_rhs[y] && x < shortest_set_size) out_result.push_back(in_lhs[x++]);
            while (in_rhs[y] < in_lhs[x] && y < shortest_set_size) out_result.push_back(in_rhs[y++]);
            if (x == shortest_set_size)
            {
                x++;
                y++;
            }
            else
            {
                break; // reached the end
            }
        }
    }

}

#endif // !DRAKO_JOIN_HPP
