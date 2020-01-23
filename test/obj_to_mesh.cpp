#include <filesystem>
#include <fstream>
#include <iostream>

#include "drako/plugins/wavefront_parser.hpp"

const auto HELP_TEXT = "Usage: program [input]";

int main(int argc, char* argv[])
{
    using namespace drako::plugins::wavefront;

    if (argc == 1)
    {
        std::cout << HELP_TEXT;
    }

    const std::filesystem::path src_path{ argv[1] };
    const std::filesystem::path dst_path{ argv[2] };

    std::ifstream src_file{ src_path };
    if (!src_file)
    {
        std::cout << "Failed to open source " << src_path << '\n';
        return EXIT_FAILURE;
    }

    std::error_code ec;
    const auto      file_size = std::filesystem::file_size(src_path, ec);
    if (ec != std::error_code{})
        return EXIT_FAILURE;

    std::vector<char> buffer(file_size);
    if (!src_file.read(buffer.data(), size(buffer)))
        return EXIT_FAILURE;

    std::ofstream dst{ dst_path };
    if (!dst)
    {
        std::cout << "Failed to open destination " << dst_path << '\n';
        return EXIT_FAILURE;
    }

    obj_parser_config config{};
    config.expected_object_count   = 1;
    config.expected_vertex_count   = 1000;
    config.expected_triangle_count = 3000;

    const auto [result, warnings, errors] = parse(std::string_view{ buffer.data(), buffer.size() }, config);

    if (!std::empty(warnings))
    {
        for (const auto& w : warnings)
        {
            std::cout << "[WARNING]" << to_string(w.warc) << '\n';
        }
    }

    if (!std::empty(errors))
    {
        for (const auto& e : errors)
        {
            const std::string_view src_error_line{ buffer.data() + e.char_index(), 10 };
            std::cerr << src_path << ':' << e.line() << '.' << e.column()
                      << ": error " << e.value() << " " << e.message() << '\n'
                      << src_error_line << '\n';
        }
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "Vertex count: " << std::size(result.vertex_points) << '\n'
                  << "Normals count: " << std::size(result.vertex_normals) << '\n'
                  << "Texcoords count: " << std::size(result.vertex_texcoords) << '\n';
    }
}