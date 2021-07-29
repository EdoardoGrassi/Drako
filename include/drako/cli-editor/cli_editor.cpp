#include "drako/core/compiler.hpp"
#include "drako/devel/mesh_importers.hpp"
#include "drako/devel/project_types.hpp"
#include "drako/devel/project_utils.hpp"

#include "drako/devel/editor_system.hpp"

#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <span>
#include <string>
#include <vector>

using namespace drako;
using namespace drako::editor;

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
    AssetImportStack              stack;
    std::optional<ProjectContext> project;
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

void print_on_console(const editor::AssetImportError& e)
{
    std::cout << "[!] error while importing file " << e.file << " :\n"
              << e.message << '\n';
}

void cmd_exit(const CmdArgs&, const CmdContext&)
{
    std::exit(EXIT_SUCCESS);
}

void cmd_create_project(const CmdArgs& a, CmdContext& c)
{
    if (std::size(a) != 2)
        throw std::runtime_error{ "Invalid argument count" };

    const auto& name = a[0];
    const auto& root = a[1];

    std::cout << "Creating new project at " << _fs::absolute(root) << '\n';

    if (!_fs::create_directories(root))
        throw std::runtime_error{ "Directory already in use" };

    const ProjectManifest pm{ .name = name, .author = "<unknown>" };
    c.project.emplace(root, pm);

    //std::cout << "root:" << c.project->root() << '\n';
    std::cout << "Project created.\n ";
}

void cmd_open_project(const CmdArgs& a, CmdContext& c)
{
    if (std::size(a) != 1)
        throw std::runtime_error{ "Invalid argument count" };

    const auto& root = a[0];
    std::cout << "Opening project at" << _fs::absolute(root) << "...\n";

    if (!_fs::exists(root))
        throw std::runtime_error{ "Directory does not exist" };
    c.project.emplace(root);

    std::cout << "root:" << c.project->root() << '\n';
}

void cmd_import_asset(const CmdArgs& args, const CmdContext& ctx)
{
    const auto& file = args[0];
    const auto& proj = ctx.project.value();

    const AssetImportInfo info{
        .guid = proj.generate_asset_uuid(),
        .path = file,
    };

    //(proj, info, ctx.stack);
    std::cout << "Imported asset with ID " << info.guid.string() << '\n';
}

void cmd_scan_assets(const CmdArgs& args, const CmdContext& ctx)
{
    const auto& project = ctx.project.value();
    std::cout << "Scanning "
              << _fs::absolute(asset_meta_directory(project))
              << " ...\n";

    const auto scan = scan_all_assets(project);

    std::cout << "[ ] found " << std::size(scan.assets) << " assets:\n";
    for (const auto& a : scan.assets)
    {
        std::cout << "\tname:    " << a.name << '\n'
                  << "\tguid:    " << a.guid << '\n'
                  << "\tpath:    " << a.path << '\n'
                  << "\tversion: " << a.version.string() << "\n\n";
    }

    if (!std::empty(scan.errors))
    {
        std::cout << "[!] " << std::size(scan.errors) << " errors occurred:\n";
        for (const auto& e : scan.errors)
            std::cout << "\tpath: " << e.file << '\n'
                      << "\tinfo: " << e.message << "\n\n";
    }
}

void cmd_scan_bundles(const CmdArgs& args, const CmdContext& ctx)
{
    const auto& project = ctx.project.value();
    std::cout << "Scanning "
              << _fs::absolute(bundle_meta_directory(project))
              << " ...\n";

    const auto scan = scan_all_bundles(project);

    std::cout << "[ ] found " << std::size(scan.bundles) << " bundles:\n";
    for (const auto& b : scan.bundles)
    {
        std::cout << "\tname: " << b.name << '\n'
                  << "\tguid: " << b.guid << '\n';
    }

    if (!std::empty(scan.errors))
    {
        std::cout << "[!] " << std::size(scan.errors) << " errors occurred:\n";
        for (const auto& e : scan.errors)
            std::cout << "\tpath: " << e.file << '\n'
                      << "\tinfo: " << e.message << "\n\n";
    }
}


int main(const int argc, const char* argv[])
{
    const std::vector<Command> commands = {
        {
            .name = "create",
            .info = "creates a new project with name [name] at location [path-to-dir]",
            .args = { "name", "path-to-dir" },
            .call = cmd_create_project,
        },
        {
            .name = "exit",
            .info = "closes the editor",
            .args = {},
            .call = cmd_exit,
        },
        {
            .name = "import",
            .info = "imports the file [path-to-file] as an asset in the project",
            .args = { "path-to-file" },
            .call = cmd_import_asset,
        },
        {
            .name = "open",
            .info = "opens an existing project",
            .args = { "path-to-dir" },
            .call = cmd_open_project,
        },
        {
            .name = "scan-assets",
            .info = "scans the assets metadata folder and prints the result",
            .args = {},
            .call = cmd_scan_assets,
        },
        {
            .name = "scan-bundles",
            .info = "scans the bundles metadata folder and prints the result",
            .args = {},
            .call = cmd_scan_assets,
        },
    };

    std::cout << PROGRAM_HEADER << "\n\n";
    if (argc == 2 && argv[1] == "--help")
    { // print program help
        std::cout << PROGRAM_HELPER << "\n\n";
        std::exit(EXIT_SUCCESS);
    }

    std::cout << "Available commands (format is [command] [[arg0] ...]):\n\n";
    for (const auto& c : commands)
    {
        std::cout << "\t" << c.name;
        for (const auto& a : c.args)
            std::cout << " [" << a << ']';
        std::cout << '\n';
        std::cout << '\t' << c.info << "\n\n ";
    }

    std::map<std::string_view, CmdFunction> mappings{};
    for (const auto& c : commands)
        mappings[c.name] = c.call;

    ;
    const AssetImportStack stack = {
        { ".obj", import_asset_obj }
    };
    CmdContext context{ .stack = stack };

    std::cout << ">>> ";
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
        std::cout << "\n>>>";
    }
}