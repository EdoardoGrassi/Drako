#include "drako/file_formats/dson/src/dson_lexer.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace drako::dson::detail;

using TT = Token::Type;

GTEST_TEST(Lexer, SpaceSkip)
{
    const char text[] = "aaa  bbb  ccc  ddd";
    const auto ts     = lex(text);

    ASSERT_TRUE(std::size(ts) == 4);
    ASSERT_EQ(ts[0].view(), "aaa");
    ASSERT_EQ(ts[1].view(), "bbb");
    ASSERT_EQ(ts[2].view(), "ccc");
    ASSERT_EQ(ts[3].view(), "ddd");

    ASSERT_EQ(ts[0].type, TT::identifier);
    ASSERT_EQ(ts[1].type, TT::identifier);
    ASSERT_EQ(ts[2].type, TT::identifier);
    ASSERT_EQ(ts[3].type, TT::identifier);
}

GTEST_TEST(Lexer, LineSkip)
{
    const char text[] = "\n\n\n\n\n";
    const auto ts     = lex(text);

    ASSERT_TRUE(std::size(ts) == 5);
    for (const auto& t : ts)
        ASSERT_EQ(t.view(), "\n");
    for (const auto& t : ts)
        ASSERT_EQ(t.type, TT::terminator);
}

GTEST_TEST(Lexer, KeyValuePairs)
{
    const char text[] = "key1=val1\n"
                        "key2=val2\n"
                        "key3=val3\n";
    const auto ts = lex(text);

    ASSERT_TRUE(std::size(ts) == 12);
    {
        const std::string expected[] = {
            "key1", "=", "val1", "\n",
            "key2", "=", "val2", "\n",
            "key3", "=", "val3", "\n"
        };
        for (auto i = 0; i < std::size(ts); ++i)
            ASSERT_EQ(ts[i].view(), expected[i]);
    }
    {
        const TT expected[] = {
            TT::identifier, TT::separator, TT::identifier, TT::terminator,
            TT::identifier, TT::separator, TT::identifier, TT::terminator,
            TT::identifier, TT::separator, TT::identifier, TT::terminator
        };
        for (auto i = 0; i < std::size(ts); ++i)
            ASSERT_EQ(ts[i].type, expected[i]);
    }
}

GTEST_TEST(Lexer, CommentSkip)
{
    const char text[] = "  # this should all be treated as a comment\n"
                        "  # even this special character = here     \n";
    const auto ts = lex(text);
    ASSERT_TRUE(std::size(ts) == 0);
}