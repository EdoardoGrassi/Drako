#include "drako/file_formats/dson/src/dson_lexer.hpp"

#include <cassert>
#include <span>
#include <stdexcept>
#include <vector>

namespace drako::dson::detail
{
    struct LexerFSA
    {
        constexpr LexerFSA() noexcept
        {
            using _ls = LexerState;

            const unsigned char INPUT_END      = '\0';
            const unsigned char COMMENT_BEG    = '#';
            const unsigned char SEPARATOR      = '=';
            const unsigned char LINEFEED       = '\n';
            const unsigned char WHITESPACE[]   = { ' ' };
            const unsigned char IDENTIFIER_BEG = '!'; // first printable ASCII character
            const unsigned char IDENTIFIER_END = '~'; // last printable ASCII character

            in_token[_ls::char_seq]         = 1;
            in_token[_ls::final_separator]  = 1;
            in_token[_ls::final_terminator] = 1;

            std::fill_n(&fsa[0][0], std::size(fsa), _ls::final_error);

            for (const auto c : WHITESPACE)
                fsa[_ls::whitespace][c] = _ls::whitespace;
            for (auto c = IDENTIFIER_BEG; c != IDENTIFIER_END; ++c)
                fsa[_ls::whitespace][c] = _ls::char_seq;
            //^^^ regular cases ^^^/vvv special cases vvv
            fsa[_ls::whitespace][COMMENT_BEG] = _ls::comment;
            fsa[_ls::whitespace][SEPARATOR]   = _ls::final_separator;
            fsa[_ls::whitespace][LINEFEED]    = _ls::final_terminator;
            fsa[_ls::whitespace][INPUT_END]   = _ls::final_input_end;

            std::fill(std::begin(fsa[_ls::comment]), std::end(fsa[_ls::comment]), _ls::comment);
            fsa[_ls::comment][LINEFEED]  = _ls::whitespace; // end of COMMENT_BEG line
            fsa[_ls::comment][INPUT_END] = _ls::final_input_end;

            for (auto c = IDENTIFIER_BEG; c != IDENTIFIER_END; ++c)
                fsa[_ls::char_seq][c] = _ls::char_seq;
            for (const auto c : WHITESPACE)
                fsa[_ls::char_seq][c] = _ls::final_identifier;
            //^^^ regular cases ^^^/vvv special cases vvv
            fsa[_ls::char_seq][INPUT_END]   = _ls::final_identifier;
            fsa[_ls::char_seq][SEPARATOR]   = _ls::final_identifier;
            fsa[_ls::char_seq][LINEFEED]    = _ls::final_identifier;
            fsa[_ls::char_seq][COMMENT_BEG] = _ls::final_identifier;
        }

        LexerState fsa[3][256];
        int        in_token[LexerState::last_state] = {};
    };

    /*
    [[nodiscard]] std::initializer_list<LexerState> _build_lexer_table() noexcept
    {
        using _ls = LexerState;

        const unsigned char INPUT_END    = '\0';
        const unsigned char COMMENT_BEG      = '#';
        const unsigned char SEPARATOR    = '=';
        const unsigned char LINEFEED   = '\n';
        const unsigned char whitespace[] = { ' ' };
        const unsigned char identifier[] = "abcdefghilmnopqrstuvzABCD";

        const int is_token_state[] = {
            0,
            1,
            1,
            1
        };

        _ls fsa[3][256] = {};
        std::fill_n(&fsa[0][0], std::size(fsa), _ls::final_error);

        for (const auto c : whitespace)
            fsa[_ls::whitespace][c] = _ls::whitespace;
        for (const auto c : identifier)
            fsa[_ls::whitespace][c] = _ls::char_seq;
        fsa[_ls::whitespace][COMMENT_BEG]    = _ls::COMMENT_BEG;
        fsa[_ls::whitespace][SEPARATOR]  = _ls::final_separator;
        fsa[_ls::whitespace][LINEFEED] = _ls::final_terminator;

        std::fill(std::begin(fsa[_ls::COMMENT_BEG]), std::end(fsa[_ls::COMMENT_BEG]), _ls::COMMENT_BEG);
        fsa[_ls::COMMENT_BEG][LINEFEED] = _ls::whitespace; // end of COMMENT_BEG line

        for (const auto c : identifier)
            fsa[_ls::char_seq][c] = _ls::char_seq;
        for (const auto c : whitespace)
            fsa[_ls::char_seq][c] = _ls::final_identifier;

    }*/

    [[nodiscard]] std::vector<Token> lex(std::span<const char> s)
    {
        using TT  = Token::Type;
        using _ls = LexerState;

        assert(std::end(s)[-1] == '\0'); // input buffer end mark

        const static LexerFSA lexer{};

        std::vector<Token> tokens;
        tokens.reserve(1024);

        _ls state;
        for (auto pos = std::data(s); pos != std::data(s) + std::size(s);)
        {
            state = _ls::start_state;
            // count valid characters and skip whitespace
            auto len = 0;
            do
            {
                const auto c = static_cast<unsigned char>(*pos++);
                state        = lexer.fsa[state][c];
                len += lexer.in_token[state];
            } while (state < _ls::first_final_state);
            // either we recognized a token or we reached the end of the input stream
            assert((len > 0) || (state == _ls::final_input_end));

            switch (state)
            {
                case _ls::final_identifier:
                    --pos; // rollback last character
                    tokens.push_back({ .first = pos - len, .last = pos, .type = TT::identifier });
                    break;

                case _ls::final_separator:
                    tokens.push_back({ .first = pos - 1, .last = pos, .type = TT::separator });
                    break;

                case _ls::final_terminator:
                    tokens.push_back({ .first = pos - 1, .last = pos, .type = TT::terminator });
                    break;

                case _ls::final_input_end: // reached end of input
                    return tokens;

                case _ls::final_error:
                    throw std::runtime_error{ "Lexer error." };

                default:
                    throw std::logic_error{ "Unexpected lexer state." };
            }
        }
        return tokens;
    }
} // namespace drako::dson::detail