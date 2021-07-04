#include "drako/devel/asset_bundle_manifest.hpp"

#include "gtest/gtest.h"

#include <sstream>

using namespace drako;

GTEST_TEST(AssetBundle, Serialization)
{
    std::stringstream s{}; // proxy for a file

    AssetBundleManifest before{};
    s << before;

    AssetBundleManifest after{};
    s >> after;
}