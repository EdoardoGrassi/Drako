#include "drako/devel/project_types.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>

using namespace drako::editor;
namespace _fs = std::filesystem;

GTEST_TEST(AssetImportInfo, TextSerialization)
{
    YAML::Node yaml{};

    const AssetImportInfo before{};
    yaml << before;

    AssetImportInfo after{};
    yaml >> after;

    ASSERT_EQ(before, after);
}

GTEST_TEST(ProjectManifest, TextSerialization)
{
    YAML::Node yaml{};

    const ProjectManifest before{};
    yaml << before;

    ProjectManifest after{};
    yaml >> after;

    ASSERT_EQ(before, after);
}

GTEST_TEST(ProjectContext, Create)
{
    const auto temp = _fs::temp_directory_path();

    // create new project
    const ProjectManifest pm{
        .name   = "editor-test-project",
        .author = "editor-test-team",
    };

    const auto root = temp / pm.name;
    _fs::create_directory(root);
    try
    {
        const ProjectContext pc{ root, pm };

        //ASSERT_TRUE(_fs::is_directory(root));
        EXPECT_TRUE(_fs::is_directory(root / "assets-data")) << root;
        EXPECT_TRUE(_fs::is_directory(root / "assets-meta"));
        EXPECT_TRUE(_fs::is_directory(root / "bundles-data"));
        EXPECT_TRUE(_fs::is_directory(root / "bundles-meta"));

        // cleanup
        _fs::remove_all(root);
    }
    catch (...)
    {
        _fs::remove_all(root);
    }
}