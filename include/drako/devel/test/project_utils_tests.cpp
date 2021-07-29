#include "drako/devel/project_types.hpp"
#include "drako/devel/project_utils.hpp"

#include <gtest/gtest.h>
#include <uuid-cpp/uuid.hpp>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <random>

using namespace drako::editor;
namespace fs = std::filesystem;


struct ScopedProject
{
    explicit ScopedProject(const fs::path& root)
        : root{ fs::temp_directory_path() / "devel-test-proj" }
        , pm{ .name = "devel-utils-test", .author = "devel-team" }
        , pc{ root, pm }
    {
    }

    ~ScopedProject()
    {
        fs::remove_all(fs::temp_directory_path() / pm.name);
    }

    fs::path        root;
    ProjectManifest pm;
    ProjectContext  pc;
};


class ScopedDirectory
{
public:
    explicit ScopedDirectory(const fs::path& p)
        : _path{ p }
    {
        fs::create_directories(_path);
    }

    ~ScopedDirectory()
    {
        fs::remove_all(_path);
    }

    [[nodiscard]] const fs::path& path() const { return _path; }

private:
    fs::path _path;
};


[[nodiscard]] fs::path temp_project_directory()
{
    return fs::temp_directory_path() / ("drako_project_" + std::to_string(std::random_device()()));
}

[[nodiscard]] ProjectContext temp_project()
{
    return ProjectContext{ fs::temp_directory_path() };
}

GTEST_TEST(Util, CreateAsset)
{
    const ScopedDirectory temp{ fs::temp_directory_path() / "devel-test-proj" };
    const ProjectContext  pc{ temp.path(), {} };

    const AssetImportInfo info
    {
        .guid = uuid::SystemEngine{}()
    };
    EXPECT_NO_THROW(create_asset_meta(pc, info));

    EXPECT_TRUE(fs::is_regular_file(temp.path() / "assets-meta" / guid_to_datafile(info.guid)));
}

GTEST_TEST(Util, AssetImportSerializationCycle)
{
    const fs::path file = fs::temp_directory_path() / "drako_meta";

    const uuid::SystemEngine gen{};
    const AssetImportInfo    before{
        .guid    = gen(),
        .path    = std::to_string(std::random_device{}()),
        .name    = std::to_string(std::random_device{}()),
        .version = drako::current_api_version()
    };

    try
    {
        save_text_format(file, before);

        AssetImportInfo after{};
        load_text_format(file, after);

        EXPECT_EQ(before, after);

        fs::remove(file);
    }
    catch (...)
    {
        fs::remove(file);
        throw;
    }
}

GTEST_TEST(Util, AssetScan)
{
    const auto tempdir = temp_project_directory();
    try
    {
        fs::create_directory(tempdir);

        const uuid::SystemEngine gen{};

        AssetImportInfo before[10];
        for (auto i = 0; i < std::size(before); ++i)
        {
            before[i] = {
                .guid    = gen(),
                .path    = "/assets/asset" + std::to_string(i),
                .name    = "asset" + std::to_string(i),
                .version = drako::current_api_version()
            };
        }

        // create metafiles on disk
        for (auto i = 0; i < std::size(before); ++i)
            save_text_format(tempdir / ("asset_file" + std::to_string(i)), before[i]);

        AssetImportInfo after[std::size(before)];
        for (auto i = 0; i < std::size(after); ++i)
            load_text_format(tempdir / ("asset_file" + std::to_string(i)), after[i]);

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
}

GTEST_TEST(Util, FullAssetScan)
{
    const auto tempdir = temp_project_directory();
    try
    {
        fs::create_directory(tempdir);
        ProjectContext ctx{ tempdir };

        //create_asset(ctx, )
        const auto scan = scan_all_assets(ctx);

        // cleanup
        fs::remove_all(tempdir);
    }
    catch (...)
    {
        fs::remove_all(tempdir);
        throw;
    }
}