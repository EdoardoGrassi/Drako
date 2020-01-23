#include "drako/plugins/json.hpp"

#include <string_view>
#include <charconv>

namespace drako::plugins
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

    const char WS[] = { '\x20', '\x09', '\x0a', '\x0d' };

    inline const constexpr auto JSON_FALSE = "false";
    inline const constexpr auto JSON_TRUE  = "true";
    inline const constexpr auto JSON_NULL  = "null";
    static_assert(JSON_FALSE == "\x66\x61\x6c\x73\x65"); // false = %x66.61.6c.73.65   ; false
    static_assert(JSON_TRUE == "\x74\x72\x75\x65");      // true  = %x74.72.75.65      ; true
    static_assert(JSON_NULL == "\x6e\x75\x6c\x6c");      // null  = %x6e.75.6c.6c      ; null

    const char JSON_QUOTE_MARK = '"';
    static_assert(JSON_QUOTE_MARK == '\x22'); // quotation-mark = %x22      ; "


    std::string_view _find_next_token(const std::string_view v) noexcept
    {
        const auto tk_beg = v.find_first_not_of(WS);
        const auto tk_end = v.find_first_of(WS, tk_beg + 1);
        return v.substr(tk_beg, tk_end - tk_beg);
    }

    void parse_json(std::string_view json, const json_parser_config& config) noexcept
    {
        while (true)
        {
            const auto tk_beg = json.find_first_not_of(WS);
            const auto tk_end = json.find_first_of(WS, tk_beg + 1);
            const auto token  = json.substr(tk_beg, tk_end - tk_beg);

            if (std::size(token) == 1 && token[0] == JSON_BEGIN_OBJECT)
            {
                break;
            }

            if (std::size(token) == 1 && token[0] == JSON_BEGIN_ARRAY)
            {
                break;
            }
        }
    }

    void _parse_json_array() noexcept
    {
    }

    void _parse_json_object() noexcept
    {
    }

    void _parse_json_number() noexcept
    {
    }

} // namespace drako::plugins