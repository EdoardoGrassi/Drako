#include "drako/core/compiler.hpp"
#include "drako/devel/build_utils.hpp"
#include "drako/devel/mesh_importers.hpp"
#include "drako/devel/project_utils.hpp"
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

const auto PROGRAM_HEADER = "[DRAKO] Project manager console"
                            "(built with " DRAKO_CC_VERSION ", " __DATE__ ", " __TIME__ ")";

const auto PROGRAM_HELPER = "Usage: [COMMAND] [ARGS]...\n"
                            "Commands:\n"
                            "\tcreate <name> <where>\t\tCreate a new project <name> located at <where>.\n";


struct _tokenize_line_result
{
    std::string_view              command;
    std::vector<std::string_view> args;
};

[[nodiscard]] _tokenize_line_result lex_cmd_line(const std::string_view line)
{
    std::vector<std::string_view> tokens;

    const auto begin = std::cbegin(line);
    const auto end   = std::cend(line);

    for (auto c = 0; c < std::size(line);)
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

struct CmdArgs
{
    std::vector<std::string_view> args;
};

void cli_exit(const CmdArgs params)
{
    std::exit(EXIT_SUCCESS);
}

void cli_create_project(const CmdArgs params)
{
    using namespace drako::editor;

    const auto& name  = params.args[0];
    const auto& where = params.args[1];

    const ProjectContext ctx{ where };
    std::cout << "Creating project " << name << " at " << where << '\n';
    try
    {
        create_project_tree(ctx, where);
        std::cout << "Project created\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << '\n';
        std::cout << "Project creation failed\n";
    }
}

void cli_open_project(const std::filesystem::path& dir)
{
    using namespace drako::editor;

    const ProjectContext ctx{ dir };
    try
    {
        ProjectManifest proj_info;
        std::cout << "Name: " << proj_info.name << '\n';

        ProjectDatabase db{ ctx };
        //load_all_assets(ctx, db);
        //std::cout << "Located assets count: " << std::size(db.assets.ids) << '\n';
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << '\n';
    }

    /*
    for (std::string input;;)
    {
        std::cin >> input;
        try
        {
            const auto [cmd, args] = _tokenize_command_line(input);
            if (const auto it = commands.find(cmd); it != std::cend(commands))
            {
                std::invoke(it->second, CmdArgs{ args });
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
    */
}

void cli_scan_assets(const std::filesystem::path& dir)
{
    using namespace drako::editor;
    const ProjectContext ctx{ dir };

    std::cout << "Scanning directory " << ctx.meta_directory() << " ...\n";
    const auto scan = ctx.scan_all_assets();

    std::cout << "# assets:\n";
    for (const auto& a : scan.assets)
    {
        std::cout << "name:    " << a.id << '\n'
                  << "uuid:    " << a.id << '\n'
                  << "path:    " << a.path << '\n'
                  << "version: " << a.version.string() << "\n\n";
    }

    std::cout << "# errors:\n";
    for (const auto& e : scan.errors)
        std::cout << "path: " << e.asset << '\n'
                  << "info: " << e.message << '\n\n';
}


int main(const int argc, const char* argv[])
{
    using CmdFunction = std::function<void(const CmdArgs)>;

    /*
    const std::unordered_map<std::string_view, drako::editor::asset_import_function> importers{
        { ".obj", drako::editor::import_obj_file }
    };
    */

    const std::unordered_map<std::string_view, CmdFunction> commands = {
        { "create", cli_create_project }
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

    if (argc == 4 && argv[1] == "create")
    {
        cli_create_project({ .args = { argv[2], argv[3] } });
    }

    if (argc == 3 && argv[1] == "open")
    {
        cli_open_project(argv[2]);
    }
}