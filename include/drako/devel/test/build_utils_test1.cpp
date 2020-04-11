#include "drako/devel/assertion.hpp"
#include "drako/devel/build_utils.hpp"

#include <string>

int main()
{
    {
        const std::string buffer = "as";

        const auto crc = drako::crc32c(reinterpret_cast<const std::byte*>(buffer.data()),
            std::size(buffer) * sizeof(std::string::value_type));

        DRAKO_ASSERT(crc == 10);
    }
}