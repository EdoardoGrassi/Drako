#pragma once
#ifndef DRAKO_SOA_HPP
#define DRAKO_SOA_HPP

#include <vector>

namespace drako::soa
{
    template <typename... Ts>
    void pusb_back(std::vector<Ts>&... args)
    {
    }

} // namespace drako::soa
}

#endif // !DRAKO_SOA_HPP