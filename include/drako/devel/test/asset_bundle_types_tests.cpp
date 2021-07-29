#include "drako/devel/asset_bundle_types.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <sstream>

using namespace drako;

GTEST_TEST(AssetBundleManifest, BinarySerialization)
{
    std::stringstream s{}; // proxy for a file

    AssetBundleManifest a{};
    s << a;

    AssetBundleManifest b{};
    s >> b;

    EXPECT_EQ(a.header, b.header);
    EXPECT_EQ(a.guids, b.guids);
}

GTEST_TEST(AssetBundleManifest, TextSerialization)
{
    YAML::Node yaml{}; // proxy for a file

    AssetBundleManifest a{};
    yaml << a;

    AssetBundleManifest b{};
    yaml >> b;

    EXPECT_EQ(a.header, b.header);
    EXPECT_EQ(a.guids, b.guids);
}