#include "drako/file_formats/wavefront/obj_lexer.hpp"

int main()
{
    using namespace drako;

    {
        const char[] text = "# this is a comment";
        const auto ts = obj::detail::lex(text);
    }
}