#pragma once

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/file_formats/wavefront/parser.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <variant>

const auto HELP_TEXT = "Drako obj importer (built on" __DATE__ "with" DRAKO_CC_VERSION ")\n"
                       "Usage: obj-to-mesh [OPTION]... [FILE]...\n"
                       "Import mesh data from .obj files into .dkmesh files."
                       "\n"
                       "Behaviour:\n"
                       "\t--override-output \toverrides already existings files\n"
                       "\n"
                       "Miscellaneous:\n"
                       "\t--verbose-output  \tenables verbose output\n";

/*
enum class _program_options
{
    verbose,
    override
};
*/

int main(const int argc, const char* argv[])
{
    using namespace drako::file_formats::obj;

    //_program_options options;

    size_t stats_success_count = 0;
    size_t stats_failure_count = 0;

    if (argc == 1)
    {
        std::cout << HELP_TEXT;
        std::exit(EXIT_SUCCESS);
    }

    for (auto i = 1; i < argc; ++i)
    {
        std::filesystem::path src_path{ argv[i] };
        std::filesystem::path dst_path{ src_path.replace_extension("dkmesh") };

        std::ifstream src_file{ src_path };
        if (!src_file)
        {
            std::cout << "Failed to open source " << src_path << '\n';
            ++stats_failure_count;
            continue;
        }
        else
        {
            std::cout << "[" << i << "/" << argc - 1 << "] Opening " << src_path << " ...\n";
        }

        std::error_code ec;
        const auto      file_size = std::filesystem::file_size(src_path, ec);
        if (ec != std::error_code{})
        {
            std::cout << "Failed to read from source\n";
            ++stats_failure_count;
            continue;
        }

        std::vector<char> buffer(file_size);
        if (!src_file.read(buffer.data(), size(buffer)))
        {
            std::cout << "Failed to read from source\n";
            ++stats_failure_count;
            continue;
        }

        std::ofstream dst{ dst_path };
        if (!dst)
        {
            std::cout << "Failed to open destination " << dst_path << '\n';
            ++stats_failure_count;
            continue;
        }

        parser_config config{};
        config.expected_object_count   = 1;
        config.expected_vertex_count   = 1000;
        config.expected_triangle_count = 1000 * 3;

        /*
        std::cout << "Begin parsing...\n"; 
        const auto [objects, errors] = force_parse(std::string_view{ buffer.data(), buffer.size() }, config);
        std::cout << "Parsing ended.\n";

        if (!std::empty(errors))
        {
            for (const auto& e : errors)
            {
                const std::string_view src_error_line{ buffer.data() + e.char_index(), 10 };
                std::cerr << src_path << ':' << e.line() << '.' << e.column()
                          << ": error " << e.value() << " " << e.message() << '\n'
                          << src_error_line << '\n';
            }
            ++stats_failure_count;
            continue;
        }
        else
        {
            for (const auto& object : objects)
            {
                std::cout << "Name: " << object.name << '\n'
                          << "Vertex count: " << std::size(object.vertex_points) << '\n'
                          << "Normals count: " << std::size(object.vertex_normals) << '\n'
                          << "Texcoords count: " << std::size(object.vertex_texcoords) << '\n';
            }
            ++stats_success_count;
        }
        */
        std::cout << "Begin parsing...\n";
        const auto result = parse(std::string_view{ buffer.data(), buffer.size() }, config);
        std::cout << "Parsing ended\n";

        if (const auto report = std::get_if<parser_error_report>(&result); report)
        {
            const std::string_view src_error_line{ buffer.data() + report->char_index(), 10 };
            std::cerr << src_path << ':' << report->line() << '.' << report->column()
                      << ": error " << report->value() << " " << report->message() << '\n'
                      << src_error_line << '\n';
            ++stats_failure_count;
            continue;
        }
        else if (const auto result = std::get_if<parser_result_report>(&parse_output); result)
        {
            for (const auto& object : result->objects)
            {
                std::cout << "Name: " << object.name << '\n'
                          << "Vertex count: " << std::size(object.vertex_points) << '\n'
                          << "Normals count: " << std::size(object.vertex_normals) << '\n'
                          << "Texcoords count: " << std::size(object.vertex_texcoords) << '\n';
            }
            ++stats_success_count;
        }
        else
        { // unknown variant type
            std::cout << "Unexpected parser result\n";
        }
    }

    std::cout << "Arguments:\t" << argc - 1 << '\n'
              << "Completed:\t" << stats_success_count << '\n'
              << "Failed:   \t" << stats_failure_count << '\n';
}