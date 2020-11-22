#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>

int main()
{
    const auto file     = "asasadsafsaifaosfhoahsodasuof";
    const auto temp     = std::filesystem::temp_directory_path();
    const auto fullpath = temp / file;

    std::array<std::byte, 8000> original;
    std::array<std::byte, 8000> result;
    static_assert(std::size(original) == std::size(result));

    std::fill(original.begin(), original.end(), std::byte{ 0b10101010 });
    std::copy(original.cbegin(), original.cend(), result.begin());

    {
        drako::io::output_file_handle ofs{ fullpath };
        ofs.write(original.data(), original.size());
    }

    {
        drako::io::input_file_handle ifs{ fullpath };
        ifs.read(result.data(), result.size());
    }

    std::filesystem::remove(fullpath);

    const auto ok = std::make_pair(original.cend(), result.cend());
    assert(std::mismatch(original.cbegin(), original.cend(), result.cbegin(), result.cend()) == ok);
}