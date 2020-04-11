#pragma once
#include "drako/file_formats/json.hpp"

#include "drako/devel/logging.hpp"

#include <cctype>
#include <charconv>
#include <stack>
#include <string_view>
#include <tuple>
#include <vector>

namespace drako::file_formats::json
{
    // Specification used as reference for JSON format as implemented in the current source
    // is available at https://tools.ietf.org/html/rfc8259

    const char JSON_BEGIN_ARRAY  = '[';
    const char JSON_END_ARRAY    = ']';
    const char JSON_BEGIN_OBJECT = '{';
    const char JSON_END_OBJECT   = '}';
    const char JSON_NAME_SEP     = ':';
    const char JSON_VALUE_SEP    = ',';
    static_assert(JSON_BEGIN_ARRAY == '\x5b');  // begin-array      = ws %x5B ws  ; [ left square bracket
    static_assert(JSON_BEGIN_OBJECT == '\x7b'); // begin-object     = ws %x7B ws  ; { left curly bracket
    static_assert(JSON_END_ARRAY == '\x5d');    // end-array        = ws %x5D ws  ; ] right square bracket
    static_assert(JSON_END_OBJECT == '\x7d');   // end-object       = ws %x7D ws  ; } right curly bracket
    static_assert(JSON_NAME_SEP == '\x3a');     // name-separator   = ws %x3A ws  ; : colon
    static_assert(JSON_VALUE_SEP == '\x2c');    // value-separator  = ws %x2C ws  ; , comma

    const char JSON_WHITESPACE_SET[] = { '\x20', '\x09', '\x0a', '\x0d' };

    inline const constexpr std::string_view JSON_FALSE = "false";
    inline const constexpr std::string_view JSON_TRUE  = "true";
    inline const constexpr std::string_view JSON_NULL  = "null";
    static_assert(JSON_FALSE == "\x66\x61\x6c\x73\x65"); // false = %x66.61.6c.73.65   ; false
    static_assert(JSON_TRUE == "\x74\x72\x75\x65");      // true  = %x74.72.75.65      ; true
    static_assert(JSON_NULL == "\x6e\x75\x6c\x6c");      // null  = %x6e.75.6c.6c      ; null


    const char QUOTE_MARK      = '\x22'; //     "   quotation mark  U+0022
    const char REVERSE_SOLIDUS = '\x5c'; //     \   reverse solidus U+005C
    const char SOLIDUS         = '\x2f'; //     /   solidus         U+002F
    const char BACKSPACE       = '\x62'; //     b   backspace       U+0008
    const char FORM_FEED       = '\x66'; //     f   form feed       U+000C
    const char LINE_FEED       = '\x6e'; //     n   line feed       U+000A
    const char CARRIAGE_RETURN = '\x72'; //     r   carriage return U+000D
    const char TAB             = '\x74'; //     t   tab             U+0009

    const char JSON_ESCAPED_SET[] = {
        QUOTE_MARK,
        REVERSE_SOLIDUS,
        SOLIDUS,
        BACKSPACE,
        FORM_FEED,
        LINE_FEED,
        CARRIAGE_RETURN,
        TAB
    };

    enum class _token_type
    {
        begin_array,
        end_array,
        begin_object,
        end_object,
        name_separator,
        value_separator,
        value,

        string,
        number,
        false_value,
        true_value,
        null_value
    };

    struct _token
    {
        const char16_t* begin;
        const char16_t* end;
        _token_type     type;

        explicit constexpr _token(const char16_t* begin, const char16_t* end, _token_type type) noexcept
            : begin(begin)
            , end(end)
            , type(type)
        {
        }
    };

    DRAKO_NODISCARD DRAKO_FORCE_INLINE bool _is_json_whitespace(const char ch) noexcept
    {
        return std::find(std::cbegin(JSON_WHITESPACE_SET),
                   std::cend(JSON_WHITESPACE_SET),
                   ch) != std::cend(JSON_WHITESPACE_SET);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE bool _is_json_escaped(const char ch) noexcept
    {
        return std::find(std::cbegin(JSON_ESCAPED_SET),
                   std::cend(JSON_ESCAPED_SET),
                   ch) != std::cend(JSON_ESCAPED_SET);
    }

    std::string_view _find_next_token(const std::string_view v) noexcept
    {
        const auto tk_beg = v.find_first_not_of(JSON_WHITESPACE_SET);
        const auto tk_end = v.find_first_of(JSON_WHITESPACE_SET, tk_beg + 1);
        return v.substr(tk_beg, tk_end - tk_beg);
    }


    // Parses a json encoded string converting escaped and surrogate characters.
    DRAKO_NODISCARD
    std::tuple<bool, std::string> _json_to_utf8(std::string_view json)
    {
        std::string result;

        for (auto ch = std::cbegin(json); ch != std::cend(json); std::advance(ch, 1))
        {
        }

        if (ch == REVERSE_SOLIDUS)
        {
            if (ch == 'u')
            { // identified a U+XXXX character sequence
                for (auto i = 0; i < 4; ++i)
                {
                    if (!std::isxdigit(static_cast<unsigned char>(ch[i + 1])))
                        return { false, {} };
                }
            }
            else if (_is_json_escaped(ch[1]))
            { // identified a two-character sequence (escape representation)
                result += *ch[1];
            }
            else
            { // invalid json syntax for strings
                return { false, {} };
            }
        }
        return { true, result };
    }

    DRAKO_NODISCARD
    std::string _ascii_to_json(std::string_view ascii)
    {
        std::string result;
        for (auto ch = std::cbegin(ascii); ch != std::cend(ascii); std::advance(ch, 1))
        {
            if (_is_json_escaped(*ch))
            {
                result += REVERSE_SOLIDUS;
            }
            result += *ch;
        }
        return result;
    }


    DRAKO_NODISCARD
    std::tuple<std::vector<_token>, parser_error>
    _tokenize_dom(std::u16string_view json, const parser_config& config) noexcept
    {
        using _jtk    = _token;
        using _jtk_ty = _token_type;

        std::vector<_jtk> tokens(std::size(json) / 4); // rough guess of the number of tokens

        for (const auto ch = &json[0]; ch != &json[std::size(json)]; std::advance(ch, 1))
        {
            switch (*ch)
            {
                case JSON_VALUE_SEP:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::value_separator });
                    break;

                case JSON_NAME_SEP:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::name_separator });
                    break;

                case JSON_BEGIN_ARRAY:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::begin_array });
                    break;

                case JSON_END_ARRAY:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::end_array });
                    break;

                case JSON_BEGIN_OBJECT:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::begin_object });
                    break;

                case JSON_END_OBJECT:
                    tokens.emplace_back(_jtk{ ch, ch + 1, _jtk_ty::end_object });
                    break;

                case JSON_FALSE[0]:
                {
                    if (std::distance(ch, &json[std::size(json)]) > std::size(JSON_FALSE) &&
                        std::u16string_view{ ch, std::size(JSON_FALSE) } == JSON_FALSE)
                    { // token matches keyword lenght and characters
                        tokens.emplace_back(_jtk{ ch, ch + std::size(JSON_FALSE), _jtk_ty::false_value });
                    }
                    else
                    { // lexer error
                        return { {}, {} };
                    }
                    break;
                }

                case JSON_TRUE[0]:
                {
                    if (std::distance(ch, &json[std::size(json)]) > std::size(JSON_TRUE) &&
                        std::u16string_view{ ch, std::size(JSON_TRUE) } == JSON_TRUE)
                    { // token matches keyword lenght and characters
                        tokens.emplace_back(_jtk{ ch, ch + std::size(JSON_TRUE), _jtk_ty::false_value });
                    }
                    else
                    { // lexer error
                        return { {}, {} };
                    }
                    break;
                }

                case JSON_NULL[0]:
                {
                    if (std::distance(ch, &json[std::size(json)]) > std::size(JSON_NULL) &&
                        std::u16string_view{ ch, std::size(JSON_NULL) } == JSON_NULL)
                    { // token matches keyword lenght and characters
                        tokens.emplace_back(_jtk{ ch, ch + std::size(JSON_NULL), _jtk_ty::false_value });
                    }
                    else
                    { // lexer error
                        return { {}, {} };
                    }
                    break;
                }
            }
        }
    }

    DRAKO_NODISCARD
    std::tuple<dom_tree, parser_error>
    _parse_dom(const std::vector<_token>& tokens) noexcept
    {
        enum class _parser_context
        {
            array,
            object
        };

        using _tty = _token_type;
        using _ctx = _parser_context;


        std::stack<_ctx> state;
        dom_tree_node    root;
        dom_tree_node*   curr;
        for (auto tk = std::cbegin(tokens); tk != std::cend(tokens); std::advance(tk, 1))
        {
            switch (tk->type)
            {
                case _tty::number:
                {
                    if (state.top() == _ctx::array)
                    {
                        // TODO: update dom
                        value_number value{};

                        if (tk[1].type == _tty::value_separator) // skip trailing separator
                            std::advance(tk, 1);
                    }
                    else if (state.top() == _ctx::object)
                    {
                    }
                }

                case _tty::string:
                {
                    if (state.top() == _ctx::array)
                    {
                        value_string value{};


                        if (tk[1].type == _tty::value_separator) // skip trailing separator
                            std::advance(tk, 1);
                    }
                }

                case _tty::begin_array:
                {
                    state.emplace(_ctx::array);
                    curr = std::make_unique<dom_tree_node>(value_array{});
                    break;
                }

                case _tty::begin_object:
                {
                    state.emplace(_ctx::object);
                    // TODO: update dom
                    break;
                }

                case _tty::end_array:
                {
                    if (!std::empty(state) && state.top() == _ctx::array)
                    { // matching begin-array token
                        state.pop();
                        // TODO: update dom
                        break;
                    }
                    else
                    { // parser error
                        return {};
                    }
                }

                case _tty::end_object:
                {
                    if (!std::empty(state) && state.top() == _ctx::object)
                    { // matching begin-object token
                        state.pop();
                        // TODO: update dom
                        break;
                    }
                    else
                    { // parser error
                        return {};
                    }
                }

                default:
                {
                    DRAKO_LOG_ERROR("[DRAKO] Unknown JSON token type");
                    std::exit(EXIT_FAILURE);
                }
            }
        }
    }

    DRAKO_NODISCARD
    std::tuple<dom_tree, parser_error>
    parse_dom(std::u16string_view json, const parser_config& config) noexcept;


    std::ostream& operator<<(std::ostream& os, const dom_tree_node& n)
    {
        switch (n._payload_type)
        {
            case value_type::array:
            {
                os << '[';
                for (auto i = 0; i < n._payload_array.size; ++i)
                {
                    os << ', ' << n._payload_array.values[i];
                }
                os << ']';
            }

            case value_type::object:
            {
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const dom_tree& dom)
    {
        while (true)
        {
        }
    }

} // namespace drako::file_formats::json