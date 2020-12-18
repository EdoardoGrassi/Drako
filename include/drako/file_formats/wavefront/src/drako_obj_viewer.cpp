#include "drako/core/compiler.hpp"
#include "drako/file_formats/wavefront/parser.hpp"
#include "drako/io/input_file_handle.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <variant>

constexpr const auto helper = "Viewer of .obj files\n"
                              "(built on " __DATE__ " with " DRAKO_CC_VERSION ")\n"
                              "Usage: drako-obj-viewer [FILE ...]\n";

/*
constexpr const auto helper = "Drako obj visualizer (built on " __DATE__ " with " DRAKO_CC_VERSION ")\n"
                              "Usage: obj-to-mesh [OPTION]... [FILE]...\n"
                              "Import mesh data from .obj files into .dkmesh files."
                              "\n"
                              "Behaviour:\n"
                              "\t--override-output \toverrides already existings files\n"
                              "\n"
                              "Miscellaneous:\n"
                              "\t--verbose-output  \tenables verbose output\n";*/

/*
enum class _program_options
{
    verbose,
    override
};
*/

int main(const int argc, const char* argv[])
{
    using namespace drako;
    using namespace drako::file_formats::obj;

    //_program_options options;
    struct
    {
        std::size_t success_count = 0;
        std::size_t failure_count = 0;
    } stats;

    if (argc == 1)
    {
        std::cout << helper;
        std::exit(EXIT_SUCCESS);
    }

    const parser_config config{
        .expected_object_count   = 1,
        .expected_vertex_count   = 1000,
        .expected_triangle_count = 1000 * 3
    };

    for (auto i = 1; i < argc; ++i)
    {
        try
        {
            const std::filesystem::path  path{ argv[i] };
            io::UniqueInputFile src{ path };

            const auto file_size = static_cast<size_t>(std::filesystem::file_size(path));

            auto buffer = std::make_unique<char[]>(file_size);
            io::read_exact(src, { reinterpret_cast<std::byte*>(buffer.get()), file_size });

            std::cout << "Parsing " << path << " ...\n";
            const auto result = parse({ buffer.get(), file_size }, config);
            std::cout << "Done.\n";

            std::cout << "Found " << std::size(result.objects) << " objects:\n";
            for (const auto& object : result.objects)
            {
                std::cout << '\t' << object.name << '\n';
            }

            std::cout << "Found " << std::size(result.groups) << " groups:\n";
            for (const auto& group : result.groups)
            {
                std::cout << '\t' << group.name << '\n';
            }

            ++stats.success_count;
        }
        catch (const std::exception& e)
        {
            std::cout << "Error: " << e.what() << '\n';
            ++stats.failure_count;
        }
    }

    std::cout << "Arguments:\t" << argc - 1 << '\n'
              << "Completed:\t" << stats.success_count << '\n'
              << "Failed:   \t" << stats.failure_count << '\n';
}