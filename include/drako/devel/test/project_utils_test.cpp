#include "drako/devel/project_utils.hpp"
#include "drako/devel/uuid_engine.hpp"

#include "gtest/gtest.h"

#include <filesystem>
#include <random>

using namespace drako::editor;
namespace fs = std::filesystem;

[[nodiscard]] fs::path temp_project_directory()
{
    return fs::temp_directory_path() / ("drako_project_" + std::to_string(std::random_device()()));
}

GTEST_TEST(ProjectDevel, AssetImportSerializationCycle)
{
    const std::string unique_name = std::tmpnam(nullptr);

    const fs::path file = fs::temp_directory_path() / ("drako_" + unique_name);

    const drako::UuidMacEngine gen{};
    const AssetImportInfo      before{
        .id      = gen(),
        .path    = std::to_string(std::random_device{}()),
        .name    = std::to_string(std::random_device{}()),
        .version = drako::current_api_version()
    };
    save_by_path(file, before);

    AssetImportInfo after{};
    load_by_path(file, after);

    ASSERT_EQ(before, after);
}

/*GTEST_TEST(ProjectDevel, AssetScan)
{
    const auto tempdir = temp_project_directory();
    try
    {
        fs::create_directory(tempdir);

        const drako::UuidMacEngine gen{};

        AssetImportInfo before[10];
        for (auto i = 0; i < std::size(before); ++i)
        {
            before[i] = { .id = gen(),
                .path         = "/assets/asset" + std::to_string(i),
                .name         = "asset" + std::to_string(i),
                .version      = drako::current_api_version() };
        }

        // create metafiles on disk
        for (auto i = 0; i < std::size(before); ++i)
            save(tempdir / ("asset_file" + std::to_string(i)), before[i]);

        AssetImportInfo after[std::size(before)];
        for (auto i = 0; i < std::size(after); ++i)
            load(tempdir / ("asset_file" + std::to_string(i)), after[i]);

        static_assert(std::size(before) == std::size(after));
        for (auto i = 0; i < std::size(before); ++i)
        {
            ASSERT_EQ(before[i], after[i]);
        }
        // cleanup
        fs::remove_all(tempdir);
    }
    catch (...)
    {
        fs::remove_all(tempdir);
        throw;
    }
}*/

GTEST_TEST(ProjectDevel, FullAssetScan)
{
    const auto tempdir = temp_project_directory();
    try
    {
        fs::create_directory(tempdir);
        ProjectContext ctx{ tempdir };
        create_project_tree(ctx, "test_project_utils");

        //create_asset(ctx, )
        const auto scan = ctx.scan_all_assets();

        // cleanup
        fs::remove_all(tempdir);
    }
    catch (...)
    {
        fs::remove_all(tempdir);
        throw;
    }
}