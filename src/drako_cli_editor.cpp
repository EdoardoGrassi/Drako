#pragma once

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/build_utils.hpp"
#include "drako/devel/mesh_importers.hpp"
#include "drako/devel/project_utils.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/file_formats/wavefront_obj.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
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
    std::vector<std::string_view> tokens;

    const auto begin = std::cbegin(line);
    const auto end   = std::cend(line);

    for (auto c = 0; c < line.size();)
    {
        const auto token_begin = line.find_first_not_of(' ', c);
        const auto token_end   = line.find_first_not_of(' ', token_begin);

        tokens.push_back(line.substr(token_begin, token_end - token_begin));
        c = token_end + 1;
    }
    if (std::empty(tokens))
        throw std::runtime_error("Empty command line");

    _tokenize_line_result result;
    result.command = tokens.front();
    result.args    = std::vector<std::string_view>(std::next(tokens.begin()), tokens.end());
    return result;
}

struct _cmd_params
{
    const std::vector<std::string_view> args;
};

void _exit(_cmd_params params)
{
    std::exit(EXIT_SUCCESS);
}

void _create_project_cmd(_cmd_params params)
{
}


int main(const int argc, const char* argv[])
{
    using _cmd_fun = std::function<void(_cmd_params)>;

    const std::unordered_map<std::string_view, drako::editor::asset_import_function> importers{
        { ".obj", drako::editor::import_obj_asset }
    };

    const std::unordered_map<std::string_view, _cmd_fun> commands = {
        { "create", _create_project_cmd }
    };

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

    drako::editor::project proj{ argv[1] };
    try
    {
        const auto proj_info = drako::editor::load_project_info(argv[1]);
        std::cout << "Name: " << proj_info.name << '\n';

        drako::editor::load_project(proj);
        std::cout << "Located assets count: " << std::size(proj.asset_table.asset_guids) << '\n';
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    for (std::string input;;)
    {
        std::cin >> input;
        try
        {
            const auto [cmd, args] = _tokenize_command_line(input);
            if (const auto it = commands.find(cmd); it != std::cend(commands))
            {
                std::invoke(it->second, _cmd_params{ args } );
            }
            else
            {
                std::cout << "Command " << cmd << " not found!\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
    }
}