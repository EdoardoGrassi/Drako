#include "drako/file_formats/wavefront/object.hpp"
#include "drako/file_formats/wavefront/parser.hpp"

#include <cassert>
#include <iostream>

int main()
{
    using namespace drako::file_formats;

    const char text[] = "# aaaaaaaaaaaaaaaaaaa\n"
                        "# bbbbbbbbbbbbbbbbbbb\n"
                        "# ccccccccccccccccccc\n"
                        "# ddddddddddddddddddd\n";

    try
    {
        auto buffer = std::make_unique<char[]>(std::size(text));
        std::copy(std::cbegin(text), std::prev(std::cend(text)), buffer.get());
        const auto result = obj::parse({ buffer.get(), std::size(text) }, {});
        assert(std::size(result.objects) == 1); // 'default' object
    }
    catch (const obj::parser_error& pe)
    {
        std::cerr << "Parsing error: " << pe.what() << '\n';
        assert(false);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        assert(false);
    }
}