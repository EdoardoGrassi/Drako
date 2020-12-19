#pragma once
#ifndef DRAKO_OBJ_LEXER_HPP
#define DRAKO_OBJ_LEXER_HPP

#include "drako/core/compiler.hpp"

#include <cassert>
#include <span>
#include <string>
#include <vector>

namespace drako::obj::detail
{
    enum LexerState : unsigned char
    {
        whitespace,               // whitespace
        start_state = whitespace, // initial state of the automata
        alphanum,                 // sequence of non-whitespace characters
        comment,                  // comment text
        after_cr,                 // hadle Windows-style line endings '\r''\n'

        last_nonfinal_state = after_cr,
        //^^^ non-final states ^^^/vvv final states vvv
        first_final_state,

        final_identifier, // identifier
        final_newline,    // locale-indipenendent end of line
        final_input_end,  // reached the end of the input sequence '\0'
        final_error,      // error states sink

        last_state = final_error
    };

    [[nodiscard]] inline std::string to_string(const LexerState& state)
    {
        using LS = LexerState;
        switch (state)
        {
            case LS::alphanum: return DRAKO_STRINGIZE(LexerState::alphanum);
            case LS::whitespace: return DRAKO_STRINGIZE(LexerState::whitespace);
            case LS::comment: return DRAKO_STRINGIZE(LexerState::comment);
            case LS::final_identifier: return DRAKO_STRINGIZE(LexerState::final_identifier);
            case LS::final_newline: return DRAKO_STRINGIZE(LexerState::final_newline);
            case LS::final_input_end: return DRAKO_STRINGIZE(LexerState::final_input_end);
            case LS::final_error: return DRAKO_STRINGIZE(LexerState::final_error);
            default: return "unknown";
        }
    }


    struct Token
    {
        enum class Type
        {
            alphanum,
            end_of_line,
        };

        const char* first;
        const char* last;
        Type        type;

        [[nodiscard]] std::string view() const
        {
            return std::string{ first, last };
        }
    };


    [[nodiscard]] inline std::string to_string(const Token::Type& type)
    {
        using TT = Token::Type;
        switch (type)
        {
            case TT::alphanum: return DRAKO_STRINGIZE(Token::Type::alphanum);
            case TT::end_of_line: return DRAKO_STRINGIZE(Token::Type::end_of_line);
            default: return "unknown";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, const Token& t)
    {
        return os << "view: " << t.view() << ", type: " << to_string(t.type);
    }

    enum LexerEquivClass
    {
        id_char,
        whitespace,
        comment, // comment begin '#'
        lf,      // line feed '\n'
        cr,      // carriage return '\r'
        end_of_input,
        invalid,

        last_valid_value = invalid
    };

    struct LexerFSA
    {
        [[nodiscard]] constexpr LexerFSA() noexcept
        {
            using _state = LexerState;
            using _class = LexerEquivClass;

            const unsigned char INPUT_END      = '\0';
            const unsigned char COMMENT_BEG    = '#';
            const unsigned char SEPARATOR      = '=';
            const unsigned char LINEFEED       = '\n';
            const unsigned char WHITESPACE[]   = { ' ' };
            const unsigned char IDENTIFIER_BEG = '!'; // first printable ASCII character
            const unsigned char IDENTIFIER_END = '~'; // last printable ASCII character

            // fill equivalence classes table
            std::fill_n(std::begin(eq_class), std::size(eq_class), _class::invalid);
            for (auto c = IDENTIFIER_BEG; c != IDENTIFIER_END; ++c)
                eq_class[c] = _class::id_char;

            eq_class[INPUT_END]  = _class::end_of_input;
            eq_class[WHITESPACE] = _class::whitespace;
            eq_class['#']        = _class::comment;
            eq_class['\n']       = _class::lf;
            eq_class['\r']       = _class::cr;

            // fill automata transition table
            std::fill_n(&fsa[0][0], std::size(fsa), _state::final_error);

            fsa[_state::whitespace][_class::whitespace] = _state::whitespace;
            fsa[_state::whitespace][_class::id_char]    = _state::alphanum;
            fsa[_state::whitespace][_class::comment]    = _state::comment;
            fsa[_state::whitespace][_class::lf]         = _state::final_newline;
            fsa[_state::whitespace][_class::cr]         = _state::after_cr;

            fsa[_state::comment][_class::comment]      = _state::comment;
            fsa[_state::comment][_class::lf]           = _state::whitespace; // end of COMMENT_BEG line
            fsa[_state::comment][_class::cr]           = _state::whitespace;
            fsa[_state::comment][_class::end_of_input] = _state::final_input_end;

            fsa[_state::alphanum][_class::id_char]      = _state::alphanum;
            fsa[_state::alphanum][_class::whitespace]   = _state::final_identifier;
            fsa[_state::alphanum][_class::end_of_input] = _state::final_identifier;
            fsa[_state::alphanum][_class::lf]           = _state::final_identifier;
            fsa[_state::alphanum][_class::cr]           = _state::final_identifier;
            fsa[_state::alphanum][_class::comment]      = _state::final_identifier;
        }

        LexerEquivClass eq_class[256];
        LexerState      fsa[3][LexerEquivClass::last_valid_value];
    };

    void normalize_line_endings(std::span<char> s) noexcept
    {
        assert(std::end(s)[-1] == '\0');

        for (auto& ch : s)
            if (*ch == '\r' && *ch[+1] == '\n')
                *ch = ' ';
    }

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
                const auto ch       = static_cast<unsigned char>(*pos++);
                const auto eq_class = lexer.eq_class[ch];
                state               = lexer.fsa[state][eq_class];
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
} // namespace drako::obj::detail

#endif // !DRAKO_OBJ_LEXER_HPP