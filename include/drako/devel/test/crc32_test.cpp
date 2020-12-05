#include "drako/devel/build_utils.hpp"

#include <cassert>
#include <string>
#include <tuple>

int main()
{
    {
        // test samples validated with http://icyberchef.com

        const std::tuple<std::string, std::uint32_t> samples[] = {
            { "Weeks of programming can save you hours of planning", 0x9a9ee430 },
            { "The computer was born to solve problems that did not exist before", 0x34d0b51a },
            { "One bad programmer can easily create two new jobs a year", 0xb16b3cce }
        };

        for (const auto& [s, c] : samples)
        {
            assert(c == drako::crc32c(
                reinterpret_cast<const std::byte*>(s.data()), std::size(s) - 1));
        }
    }
}