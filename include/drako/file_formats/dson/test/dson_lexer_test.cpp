#include "drako/file_formats/dson/src/dson_lexer.hpp"

#include <cassert>
#include <iostream>

using namespace drako::dson::detail;

int main()
{
    using TT = Token::Type;
    {
        const char text[] = "aaa  bbb  ccc  ddd";
        const auto ts     = lex(text);

        assert(std::size(ts) == 4);
        assert(ts[0].view() == "aaa");
        assert(ts[1].view() == "bbb");
        assert(ts[2].view() == "ccc");
        assert(ts[3].view() == "ddd");

        assert(ts[0].type == TT::identifier);
        assert(ts[1].type == TT::identifier);
        assert(ts[2].type == TT::identifier);
        assert(ts[3].type == TT::identifier);
    }
    {
        const char text[] = "\n\n\n\n\n";
        const auto ts     = lex(text);

        assert(std::size(ts) == 5);
        for (const auto& t : ts)
            assert(t.view() == "\n");
        for (const auto& t : ts)
            assert(t.type == TT::terminator);
    }
    {
        const char text[] = "key1=val1\n"
                            "key2=val2\n"
                            "key3=val3\n";
        const auto ts = lex(text);

        assert(std::size(ts) == 12);
        {
            const std::string expected[] = {
                "key1", "=", "val1", "\n",
                "key2", "=", "val2", "\n",
                "key3", "=", "val3", "\n"
            };
            for (auto i = 0; i < std::size(ts); ++i)
                assert(ts[i].view() == expected[i]);
        }
        {
            const TT expected[] = {
                TT::identifier, TT::separator, TT::identifier, TT::terminator,
                TT::identifier, TT::separator, TT::identifier, TT::terminator,
                TT::identifier, TT::separator, TT::identifier, TT::terminator
            };
            for (auto i = 0; i < std::size(ts); ++i)
                assert(ts[i].type == expected[i]);
        }
    }
    {
        const char text[] = "  # this should all be treated as a comment\n"
                            "  # even this special character = here     \n";
        const auto ts = lex(text);
        assert(std::size(ts) == 0);
    }
}