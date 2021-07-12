#include "drako/core/compiler.hpp"
#include "drako/devel/project_types.hpp"
#include "drako/devel/project_utils.hpp"

#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <span>
#include <string>
#include <vector>

using namespace drako;
namespace _fs = std::filesystem;

const auto PROGRAM_HEADER = "[DRAKO] Project manager console"
                            "(built with " DRAKO_CC_VERSION ", " __DATE__ ", " __TIME__ ")";

const auto PROGRAM_HELPER = "Usage: [COMMAND] [ARGS]...\n"
                            "Commands:\n"
                            "\tcreate <name> <where>\t\tCreate a new project <name> located at <where>.\n";



[[nodiscard]] std::vector<std::string> tokenize(const std::string_view line)
{
    const std::regex               re{ "\\s+" };
    const std::vector<std::string> tokens = {
        std::regex_token_iterator(std::cbegin(line), std::cend(line), re, -1),
        {}
    };
    return tokens;
}


using CmdArgs = std::span<const std::string>;

struct CmdContext
{
    std::unique_ptr<editor::ProjectContext> project;
};

using CmdFunction = std::function<void(const CmdArgs&, CmdContext&)>;

/// @brief Command for the CLI editor.
struct Command
{
    std::string              name;
    std::string              info;
    std::vector<std::string> args;
    CmdFunction              call;
};


void cli_exit(const CmdArgs&, const CmdContext&)
{
    std::exit(EXIT_SUCCESS);
}

void cli_create_project(const CmdArgs& a, CmdContext& c)
{
    using namespace drako::editor;

    if (std::size(a) != 1)
        throw std::runtime_error{ "Invalid arguments" };

    //const auto& name = a[0];
    const auto& root = a[0];

    std::cout << "Creating new project at " << root << '\n';

    if (!_fs::create_directories(root))
        throw std::runtime_error{ "Directory does not exist" };
    c.project = std::make_unique<ProjectContext>(root);

    std::cout << "root:" << c.project->root() << '\n';
}

void cli_open_project(const CmdArgs& a, CmdContext& c)
{
    using namespace drako::editor;

    if (std::size(a) != 1)
        throw std::runtime_error{ "Invalid arguments" };

    const auto& root = a[0];
    std::cout << "Opening project at " << root << '\n';

    if (!_fs::exists(root))
        throw std::runtime_error{ "Directory does not exist" };
    c.project = std::make_unique<ProjectContext>(root);

    std::cout << "root:" << c.project->root() << '\n';
}

void cli_import_asset(const CmdArgs& args, const CmdContext& ctx)
{
    const auto& file = args[0];
    const auto  id   = ctx.project->import(file);
    std::cout << "Imported file with ID " << id.string();
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
                  << "info: " << e.message << "\n\n";
}


int main(const int argc, const char* argv[])
{
    using namespace drako;

    const std::vector<Command> commands = {
        {
            .name = "create",
            .info = "create a new project",
            .args = { "path-to-dir" },
            .call = cli_create_project,
        },
        {
            .name = "exit",
            .info = "close the editor",
            .args = {},
            .call = cli_exit,
        },
        {
            .name = "import",
            .info = "import an asset in the project",
            .args = { "path-to-file" },
            .call = cli_import_asset,
        },
        {
            .name = "open",
            .info = "open an existing project",
            .args = { "path-to-dir" },
            .call = cli_open_project,
        },
    };

    std::cout << PROGRAM_HEADER << "\n\n";
    if (argc == 2 && argv[1] == "--help")
    { // print program help
        std::cout << PROGRAM_HELPER << "\n\n";
        std::exit(EXIT_SUCCESS);
    }

    std::cout << "commands:\n\n";
    for (const auto& c : commands)
    {
        std::cout << "cmd: " << c.name;
        for (const auto& a : c.args)
            std::cout << " <" << a << '>';
        std::cout << '\n';
        std::cout << '\"' << c.info << "\"\n\n";
    }

    std::map<std::string_view, CmdFunction> mappings{};
    for (const auto& c : commands)
        mappings[c.name] = c.call;

    //std::unique_ptr<editor::ProjectContext> project;
    CmdContext context;
    for (std::string line; std::getline(std::cin, line);)
    {
        const std::vector<std::string> tokens = tokenize(line);
        //for (std::string s; std::cin >> s;)
        //    tokens.push_back(s);

        if (std::empty(tokens))
            continue;

        const auto& cmd = tokens.front();
        if (cmd == "exit")
            break;

        try
        {
            if (const auto it = mappings.find(cmd); it != std::cend(mappings))
            {
                const CmdArgs args{ std::next(std::cbegin(tokens)), std::cend(tokens) };
                std::invoke(it->second, args, context);
            }
            else
            {
                std::cout << "Command " << cmd << " not found!\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}