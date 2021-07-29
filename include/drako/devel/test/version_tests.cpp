#include "drako/devel/version.hpp"

#include <gtest/gtest.h>

using namespace drako;

GTEST_TEST(Version, Construction)
{
    const Version a{ 11, 222, 3333 };
    ASSERT_EQ(a.major(), 11);
    ASSERT_EQ(a.minor(), 222);
    ASSERT_EQ(a.patch(), 3333);

    const Version b{ "11.222.3333" };
    ASSERT_EQ(b.major(), 11);
    ASSERT_EQ(b.minor(), 222);
    ASSERT_EQ(b.patch(), 3333);
}

GTEST_TEST(Version, ToString)
{
    const Version v{ 11, 222, 3333 };
    ASSERT_EQ(v.string(), "11.222.3333");
}

GTEST_TEST(Version, FromString)
{
    //ASSERT_EQ(Version{ "11.222.3333" }, Version{ "" })
}