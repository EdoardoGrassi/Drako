#pragma once

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/build_utils.hpp"
#include "drako/devel/project_utils.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/file_formats/wavefront_obj.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include "drako/devel/mesh_importers.hpp"

#include <filesystem>
#include <fstream>
#include <iostream> // std::cout, std::cerr, std::clog
#include <string>
#include <unordered_map>
#include <vector>


namespace _fs = std::filesystem;

const std::string PROGRAM_HEADER = "[DRAKO] Project manager console"
                                   "(built with " DRAKO_CC_VERSION ", " __DATE__ ", " __TIME__ ")";

const std::string PROGRAM_HELPER = "Usage: [COMMAND] [ARGS]...\n"
                                   "Commands:\n"
                                   "\timport\n"
                                   "\n"
                                   "\tbuild-packages  \tGenerates packages for runtime consumption.\n";


struct _tokenize_line_result
{
    std::string_view              command;
    std::vector<std::string_view> args;
};

_tokenize_line_result _tokenize_command_line(const std::string_view line)
{
    const auto begin = std::cbegin(line);
    const auto end   = std::cend(line);
}


int main(const int argc, const char* argv[])
{
    const std::unordered_map<std::string, drako::editor::asset_import_function> importers{
        { ".obj", drako::editor::import_obj_asset }
    };

    const std::unordered_map<std::string, void*> commands = {};

    if (argc == 1)
    {
        std::cout << PROGRAM_HEADER << '\n';
        std::exit(EXIT_SUCCESS);
    }

    if (argc == 2 && argv[1] == "--help")
    { // print program help
        std::cout << PROGRAM_HELPER << '\n';
        std::exit(EXIT_SUCCESS);
    }

    try
    {
        const auto proj_info = drako::editor::load_project_info(argv[1]);
        std::cout << "Name: " << proj_info.name << '\n';

        const auto project = drako::editor::load_project(proj_info);
        std::cout << "Located assets count: " << std::size(project.asset_uuids) << '\n';
    }
    catch (...)
    {
        std::exit(EXIT_FAILURE);
    }

    /*
    for (std::string input;;)
    {
        std::cin >> input;

        const auto [name, args] = _tokenize_command_line(input);
        if (const auto cmd = std::find(std::cbegin(commands), std::cend(commands), name);
            cmd != std::cend(commands))
        {
            std::invoke(cmd->second);
        }
        else
        {
            std::cout << "Command " << name << " not found!\n";
        }
    }
    */
}