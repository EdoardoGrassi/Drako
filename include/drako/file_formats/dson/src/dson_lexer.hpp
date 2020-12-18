#pragma once
#ifndef DRAKO_DSON_LEXER_HPP
#define DRAKO_DSON_LEXER_HPP

#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace drako::dson::detail
{
    enum LexerState : unsigned char
    {
        whitespace,               // whitespace
        start_state = whitespace, // initial state of the automata
        char_seq,                 // sequence of non-whitespace characters
        comment,                  // comment text

        first_final_state, //^^^ non-final states ^^^/vvv final states vvv

        final_identifier, // identifier (either a key or a value)
        final_separator,  // key-value separator '='
        final_terminator, // line feed '\n'
        final_input_end,  // reached the end of the input sequence '\0'
        final_error,      // error states sink

        last_state = final_error
    };

    [[nodiscard]] inline std::string to_string(const LexerState& state)
    {
        using LS = LexerState;
        switch (state)
        {
            case LS::char_seq: return "char-sequence";
            case LS::whitespace: return "white-space";
            case LS::comment: return "comment";
            case LS::final_identifier: return "final-identifier";
            case LS::final_separator: return "final-separator";
            case LS::final_terminator: return "final-terminator";
            case LS::final_input_end: return "final-input-end";
            case LS::final_error: return "final-error";
            default: return "unknown";
        }
    }


    struct Token
    {
        enum class Type
        {
            identifier,
            separator,
            terminator,
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
            case TT::identifier: return "identifier";
            case TT::terminator: return "terminator";
            case TT::separator: return "separator";
            default: return "unknown";
        }
    }

    inline std::ostream& operator<<(std::ostream& os, const Token& t)
    {
        return os << "view: " << t.view() << ", type: " << to_string(t.type);
    }

    // Extracts tokens from a null-terminated array of characters.
    [[nodiscard]] std::vector<Token> lex(std::span<const char> s);
} // namespace drako::dson::detail

#endif // !DRAKO_DSON_LEXER_HPP