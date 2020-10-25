#include "drako/file_formats/wavefront/parser.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/file_formats/wavefront/object.hpp"

#include <charconv>
#include <istream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace drako::file_formats::obj
{
    namespace details
    {
        const float            DEFAULT_VERTEX_WEIGHT   = 1.f; // default value for vertex weight (from obj specs)
        const float            DEFAULT_VERTEX_TEXCOORD = 0.f; // default value for texture coordinate (from obj specs)
        const std::string_view line_continuation       = "\x5c";
        //const std::string_view OBJ_TAG_COMMENT         = "#";
        //const std::string_view OBJ_TAG_POINT           = "v";
        //const std::string_view OBJ_TAG_NORMAL          = "vn";
        //const std::string_view OBJ_TAG_TEXCOORDS       = "vt"

        constexpr const std::string_view ignored_keywords[] = {
            "call",   // file import command
            "csh",    // UNIX shell command
            "g",      // grouping statement
            "mtllib", // material attribute
            "s",      // smoothing group statement
            "usemtl"  // material reference statement
        };

        enum class _tag_f_format // triplets of indexes v/[vt]/[vn]
        {
            unknown,       // current format is still unspecified
            format_v,      // current format is v//
            format_v_vt,   // current format is v/vt/
            format_v_vn,   // current format is v//vn
            format_v_vt_vn // current format is v/vt/vn
        };

        struct _parser_token
        {
            const char* begin;
            const char* end;
            size_t      row;
            size_t      col;
            size_t      offset;

            // explicit _parser_token() noexcept = default;

            explicit constexpr _parser_token(const char* begin, const char* end,
                size_t file_row_number, size_t file_col_number, size_t file_char_offset) noexcept
                : begin{ begin }
                , end{ end }
                , row{ file_row_number }
                , col{ file_col_number }
                , offset{ file_char_offset }
            {
            }

            explicit operator std::string_view() const noexcept
            {
                return std::string_view(begin, std::distance(begin, end));
            }
        };
        using _pt = _parser_token;

        struct _parser_line
        {
            const char* begin;
            size_t      row;
        };
        using _pl = _parser_line;

        /*
        struct _parser_local_state
        {
            std::vector<float> points;
            std::vector<float> normals;
            std::vector<float> texcoords;
        };

        struct _parser_global_state
        {
            std::vector<parser_warning_report>& warnings;
            std::vector<parser_error_report>&   errors;
        };
        */

        // construct report for line wide errors
        [[nodiscard]] parser_error_report
        _make_line_error(const _pl& _in_line, parser_error_code _in_ec) noexcept
        {
            return parser_error_report{ _in_line.row, 0, 0, _in_ec };
        }

        // construct report for token related errors
        [[nodiscard]] parser_error_report
        _make_token_error(const _pt& _in_token, parser_error_code _in_ec) noexcept
        {
            const auto row = _in_token.row;
            const auto col = _in_token.col;
            return parser_error_report{ row, col, , _in_ec };
        }

        // construct report for token related errors
        [[nodiscard]] parser_error_report
        _make_token_error(const _pl& _in_line, const _pt& _in_token, parser_error_code _in_ec) noexcept
        {
            const auto row = _in_line.row;
            const auto col = static_cast<size_t>(std::distance(_in_line.begin, _in_token.begin));
            return parser_error_report{ row, col, 0, _in_ec };
        }

        // check if a keyword is ignored by current implementation
        [[nodiscard]] bool
        _is_ignored_keyword(const std::string_view keyword) noexcept
        {
            return std::find(std::cbegin(ignored_keywords), std::cend(ignored_keywords),
                       keyword) != std::cend(ignored_keywords);
        }

        [[nodiscard]] std::string_view _find_virtual_line();

        [[nodiscard]] const char*
        _tokenize_line(const char* _in_begin, const char* _in_end, std::vector<_pt>& _out_tokens)
        {
            auto line_begin = _in_begin;
            auto line_end   = _in_begin;
            for (auto curr_line_index = 0; line_end != _in_end; ++curr_line_index)
            {
                line_end = std::find(line_begin, _in_end, '\n');
                if (line_begin == line_end) // skip empty line
                    continue;

                if (*line_begin == '#') // skip comment line
                    continue;

                for (auto ch = _in_begin; ch != line_end;)
                {
                    while (*ch == ' ') // skip leading whitespace
                        ++ch;
                    const auto token_begin = ch;

                    while (*ch != ' ' && *ch != '\n') // reach trailing whitespace or line termination
                        ++ch;
                    const auto token_end = ch;

                    // const auto col = static_cast<size_t>(std::distance(token_begin, token_end));
                    if (token_begin != token_end)
                        _out_tokens.emplace_back(_pt{ token_begin, token_end }); //, curr_line_index, col });
                }

                if (std::string_view{ _out_tokens.back() } != line_continuation)
                    break;
                // else tokenize next line
                line_begin = std::next(line_end);
            }
            return line_begin;
        }

        // parse line with starting keyword 'v' and [first, last) tokens
        [[nodiscard]] std::optional<parser_error_report>
        _try_parse_v_line(const _pl& _in_line, const std::vector<_pt>& _in_tokens, std::vector<float>& _out_v) noexcept
        {
            using _pec = parser_error_code;
            // geometric vertex statement
            // SYNTAX:      v x y z [w]
            // DEFAULT:     w = 1.0

            DRAKO_ASSERT(!std::empty(_in_tokens));
            DRAKO_ASSERT(std::string_view{ _in_tokens.front() } == "v");

            const auto V_MIN_ARGC = 3;
            const auto V_MAX_ARGC = 4;

            if (std::size(_in_tokens) < V_MIN_ARGC)
                return _make_token_error(_in_line, _in_tokens.front(), _pec::tag_v_invalid_args_count);

            if (std::size(_in_tokens) > V_MAX_ARGC)
                return _make_token_error(_in_line, _in_tokens[V_MAX_ARGC + 1], _pec::tag_v_invalid_args_count);

            std::array<float, 4> xyzw;
            std::fill(std::begin(xyzw), std::end(xyzw), DEFAULT_VERTEX_WEIGHT);
            for (auto i = 0; i < std::size(_in_tokens); ++i)
            {
                if (const auto [ptr, errc] = std::from_chars(_in_tokens[i].begin, _in_tokens[i].end, xyzw[i]);
                    errc != std::errc{})
                {
                    return _make_token_error(_in_line, _in_tokens[i], _pec::invalid_arg_format);
                }
            }
            for (auto coord : xyzw)
                _out_v.emplace_back(coord);

            return {};
        }

        void _parse_v_line(const _pl& line, const std::vector<_pt>& tokens)
        {
            using _ec = parser_error_code;

            const auto min_argc = 3;
            const auto max_argc = 4;
            if (const auto s = std::size(tokens); s < min_argc || s > max_argc)
                throw parser_error{ _ec::tag_v_invalid_args_count };

            for (auto i = 0; i < std::size(tokens); ++i)
            {
            }
            // TODO: end impl
        }

        // parse line with starting keyword 'vn' and [first, last) tokens
        [[nodiscard]] std::optional<parser_error_report>
        _try_parse_vn_line(const _pl& line, const std::vector<_pt>& tokens, std::vector<float>& vn) noexcept
        {
            using _pec = parser_error_code;

            DRAKO_ASSERT(!std::empty(tokens));
            DRAKO_ASSERT(std::string_view{ tokens.front() } == "vn");

            const auto VN_ARGC = 3;
            // NOTE: normal vector is not required to be normalized by .OBJ specs
            if (std::size(tokens) != VN_ARGC)
            {
                if (std::size(tokens) > VN_ARGC)
                    return _make_token_error(line, tokens[VN_ARGC], _pec::tag_vn_invalid_args_count);
                else
                    return _make_token_error(line, tokens.back(), _pec::tag_vn_invalid_args_count);
            }

            float xyz[VN_ARGC];
            for (auto i = 0; i < VN_ARGC; ++i)
            {
                if (const auto [ptr, err] = std::from_chars(tokens[i].begin, tokens[i].end, xyz[i]);
                    err != std::errc{})
                {
                    return _make_token_error(line, tokens[i], _pec::invalid_arg_format);
                }
            }
            for (auto coord : xyz)
                vn.emplace_back(coord);

            return {};
        }

        // parse line with starting keyword 'vt' and [first, last) tokens
        [[nodiscard]] std::optional<parser_error_report>
        _try_parse_vt_line(const _pl& line, const std::vector<_pt>& tokens, std::vector<float>& vt) noexcept
        {
            using _pec = parser_error_code;

            DRAKO_ASSERT(!std::empty(tokens));
            DRAKO_ASSERT(std::string_view{ tokens.front() } == "vt");

            const auto VT_MIN_ARGC = 1;
            const auto VT_MAX_ARGC = 3;

            if (std::size(tokens) < VT_MIN_ARGC)
                return _make_token_error(line, tokens.back(), _pec::tag_vt_invalid_args_count);

            if (std::size(tokens) > VT_MAX_ARGC)
                return _make_token_error(line, tokens[VT_MAX_ARGC + 1], _pec::tag_vt_invalid_args_count);

            float texcoords[VT_MAX_ARGC];
            std::fill(std::begin(texcoords), std::end(texcoords), DEFAULT_VERTEX_TEXCOORD);
            for (auto i = 0; i < std::size(tokens); ++i)
            {
                if (const auto [ptr, err] = std::from_chars(tokens[i].begin, tokens[i].end, texcoords[i]);
                    err != std::errc{})
                {
                    return _make_token_error(line, tokens[i], _pec::invalid_arg_format);
                }
            }
            for (auto coord : texcoords)
                vt.emplace_back(coord);

            return {};
        }

        // parse line with starting keyword 'f' and [first, last) tokens
        [[nodiscard]] std::optional<parser_error_report>
        _try_parse_f_line(const _pl& line, const std::vector<_pt>& tokens, object& obj) noexcept
        {
            using _pec = parser_error_code;

            DRAKO_ASSERT(!std::empty(tokens));
            DRAKO_ASSERT(std::string_view{ tokens.front() } == "f");

            const auto F_MIN_ARGC = 4;

            if (std::size(tokens) < F_MIN_ARGC)
                return _make_token_error(line, tokens.back(), _pec::tag_f_invalid_args_count);

            std::vector<int32_t> v(std::size(tokens));  // geometric vertices index
            std::vector<int32_t> vt(std::size(tokens)); // texture vertices index
            std::vector<int32_t> vn(std::size(tokens)); // vertex normals index

            for (auto i = 1 /*skip keyword token*/; i < std::size(tokens); ++i)
            {
                // tokenize triplet v/[vt]/[vn]
                if (std::count(tokens[i].begin, tokens[i].end, '/') != 2)
                    return _make_token_error(line, tokens[i], _pec::tag_f_invalid_args_format);

                _parser_token tk_v, tk_vt, tk_vn;
                {
                    auto c = tokens[i].begin;
                    while (*c != '/')
                        ++c;
                    tk_v = _parser_token{ tokens[i].begin, c };

                    while (*c != '/')
                        ++c;
                    tk_vt = _parser_token{ tk_v.end + 1, c };

                    tk_vn = _parser_token{ tk_vt.end + 1, tokens[i].end };
                }

                int32_t index_v, index_vt, index_vn;

                /*vvv [v/../..] vvv*/
                if (const auto [ptr, ec] = std::from_chars(tk_v.begin, tk_v.end, index_v);
                    ec != std::errc{})
                {
                    return _make_token_error(line, tokens[i], _pec::tag_f_invalid_args_format);
                }
                if (const auto i = index_v; i == 0 || std::abs(i) > std::size(obj.vertex_points))
                {
                    return _make_token_error(line, tokens[i], _pec::index_out_of_range);
                }

                /*vvv [../vt/..] vvv*/
                if (std::distance(tk_vt.begin, tk_vt.end) > 0)
                { // vt token is present
                    if (const auto [ptr, ec] = std::from_chars(tk_vt.begin, tk_vt.end, index_vt);
                        ec != std::errc{})
                    {
                        return _make_token_error(line, tokens[i], _pec::tag_f_invalid_args_format);
                    }
                    if (const auto i = index_vt; i == 0 || std::abs(i) > std::size(obj.vertex_texcoords))
                    {
                        return _make_token_error(line, tokens[i], _pec::invalid_arg_format);
                    }
                }

                /*vvv [../../vn] vvv*/
                if (std::distance(tk_vn.begin, tk_vn.end) > 0)
                { // vn token is present
                    if (const auto [ptr, ec] = std::from_chars(tk_vn.begin, tk_vn.end, index_vn);
                        ec != std::errc{})
                    {
                        return _make_token_error(line, tokens[i], _pec::tag_f_invalid_args_format);
                    }
                    if (const auto i = index_vn; i == 0 || std::abs(i) > std::size(obj.vertex_normals))
                    {
                        return _make_token_error(line, tokens[i], _pec::invalid_arg_format);
                    }
                }
                obj.faces.emplace_back(index_v);
                obj.faces.emplace_back(index_vt);
                obj.faces.emplace_back(index_vn);
            }
            return {};
        }

        // parse line with starting keyword 'o' and [first, last) tokens
        template <typename Iter>
        [[nodiscard]] std::optional<parser_error_report>
        _try_parse_o_line(Iter _in_first_token, Iter _in_last_token, object& _out_obj)
        {
            static_assert(std::is_same_v<std::iterator_traits<Iter>::value_type, _parser_token>,
                "Iterator value_type must match " DRAKO_STRINGIZE(_parser_token));

            if (std::distance(_in_first_token, _in_last_token) != 1)
                return _make_token_error(std::next(_in_first_token), _pec::tag_o_invalid_args_count);

            _out_obj = { std::string{ _in_first_token.begin, _in_last_token.end } };
            continue;
        }

        [[nodiscard]] std::variant<parser_result, parser_error_report>
        _try_parse_buffer_until_error(const char* _in_src_begin, const char* _in_src_end, const parser_config& _in_config)
        {
            using _ptk = _parser_token;
            using _pec = parser_error_code;
            using _prr = parser_result_report;

            DRAKO_ASSERT(_in_src_begin != nullptr);
            DRAKO_ASSERT(_in_src_end != nullptr);
            DRAKO_ASSERT(_in_src_begin <= _in_src_end, "Invalid iterator range");

            if (_in_src_begin == _in_src_end)
                return _prr{};

            _prr result{};

            std::vector<float>    v_points(_in_config.expected_vertex_count * 3);    // vertex geometry
            std::vector<float>    v_normals(_in_config.expected_vertex_count * 3);   // vertex normals
            std::vector<float>    v_texcoords(_in_config.expected_vertex_count * 3); // vertex texcoords
            std::vector<uint32_t> faces(_in_config.expected_triangle_count * 3);

            object            curr_object;
            std::size_t       curr_source_row = 0;
            std::vector<_ptk> tokens(10); // estimate of max token count
            for (const auto line_begin = _in_src_begin; line_begin != _in_src_end;)
            {
                tokens.clear();
                /*
                do
                {
                    ++curr_source_row;

                    const auto line_end = std::find(line_begin, _in_src_end, '\n');
                    for (auto buffer_begin = line_begin; buffer_begin != line_end;)
                    {
                        // skip leading whitespace
                        const auto token_begin = std::find_if(buffer_begin, line_end,
                            [](auto c) { return !std::isspace(static_cast<unsigned char>(c)); });

                        // reach trailing whitespace or line termination
                        const auto token_end = std::find_if(token_begin, line_end,
                            [](auto c) { return std::isspace(static_cast<unsigned char>(c)); });

                        const auto col = static_cast<size_t>(std::distance(line_begin, token_begin));
                        tokens.emplace_back(_ptk{ token_begin, token_end, curr_source_row, col });

                        buffer_begin = token_end;
                    }
                } while (std::string_view{ tokens.back() } == line_continuation);
                */
                _tokenize_line()

                    const std::string_view keyword{ tokens.front() };
                if (keyword == "v") // vertex geometric data: v x y z [w]
                {
                    if (const auto err = _parse_v_line(tokens, v_points); err)
                        return *err;
                    continue;
                }
                else if (keyword == "vn") // vertex normal data: vn x y z
                {
                    if (const auto err = _parse_vn_line(tokens, v_normals); err)
                        return *err;
                    continue;
                }
                else if (keyword == "vt") // vertex uv data: vt u [v] [w]
                {
                    if (const auto err = _parse_vt_line(tokens, v_texcoords); err)
                        return *err;
                    continue;
                }
                else if (keyword == "f") // face element: v1/[vt1]/[vn1] v2/[vt2]/[vn2] v3/[vt3]/[vn3] [v4/vt4/vn4 ...]
                {
                    if (const auto err = _parse_f_line(tokens, curr_object); err)
                        return *err;
                    continue;
                }
                else if (keyword == "o") // new object name statement: o object_name
                {
                    if (std::size(tokens) != 2)
                        return _make_token_error(tokens.front(), _pec::tag_o_invalid_args_count);

                    result.objects.emplace_back(curr_object);
                    curr_object = object{ std::string{ tokens[0].begin, tokens[0].end } };
                    continue;
                }

                if (_is_ignored_keyword(keyword))
                    continue; // skip current line

                return _make_token_error(tokens.front(), _pec::unknown_tag); // unrecognized keyword
            }

            return result;
        }

        [[nodiscard]] std::tuple<parser_result, std::vector<parser_error_report>>
        _try_parse_buffer_skip_errors(const char* src_begin, const char* src_end, const parser_config& cfg)
        {
            using _ptk = _parser_token;
            using _pec = parser_error_code;

            DRAKO_ASSERT(src_begin != nullptr);
            DRAKO_ASSERT(src_end != nullptr);
            DRAKO_ASSERT(src_begin <= src_end, "Invalid iterator range");

            // TODO: review impl
            if (src_begin == src_end)
                return { {}, {} };

            std::vector<parser_error_report> errors; // logs error generated by parsing
            std::vector<object>              objects;
            std::vector<float>               v_points(cfg.expected_vertex_count * 3);    // vertex geometry
            std::vector<float>               v_normals(cfg.expected_vertex_count * 3);   // vertex normals
            std::vector<float>               v_texcoords(cfg.expected_vertex_count * 3); // vertex texcoords
            std::vector<uint32_t>            faces(cfg.expected_triangle_count * 3);

            object            curr_object;
            std::size_t       curr_line_index = 1;
            std::vector<_ptk> tokens(100);
            for (const auto line_begin = src_begin; line_begin != src_end;)
            {
                _tokenize_line(line_begin, line_end, tokens);

                const std::string_view keyword{ tokens.front() };
                if (keyword == "v") // vertex geometric data: v x y z [w]
                {
                    if (const auto err = _parse_v_line(tokens, v_points); err)
                        errors.emplace_back(*err);
                    continue;
                }
                else if (keyword == "vn") // vertex normal data: vn x y z
                {
                    if (const auto err = _parse_vn_line(tokens, v_normals); err)
                        errors.emplace_back(*err);
                    continue;
                }
                else if (keyword == "vt") // vertex uv data: vt u [v] [w]
                {
                    if (const auto err = _parse_vt_line(tokens, v_texcoords); err)
                        errors.emplace_back(*err);
                    continue;
                }
                else if (keyword == "f") // face element: v1/[vt1]/[vn1] v2/[vt2]/[vn2] v3/[vt3]/[vn3] [v4/vt4/vn4 ...]
                {
                    if (const auto err = _parse_f_line(tokens, curr_object); err)
                        errors.emplace_back(*err);
                    continue;
                }
                else if (keyword == "o") // new object name statement: o object_name
                {
                    if (std::size(tokens) == 2)
                    {
                        objects.emplace_back(curr_object);
                        curr_object = object{ std::string{ tokens[0].begin, tokens[0].end } };
                    }
                    else
                        errors.emplace_back(_make_token_error(tokens.front(), _pec::tag_o_invalid_args_count));

                    continue;
                }

                if (_is_ignored_keyword(keyword))
                    continue; // skip current line

                errors.emplace_back(_make_token_error(tokens.front(), _pec::unknown_tag)); // unrecognized keyword
            }
            return { objects, errors };
        }
    } // namespace details


    parser_result parse(const std::string_view source, const parser_config& config)
    {
        for (;;)
        {
            // find next virtual line

            // parse line

            // modify current entity
        }
    }


    [[nodiscard]] std::variant<parser_result, parser_error_report>
    try_parse(const std::string_view source, const parser_config& config) noexcept
    {
        return details::_try_parse_buffer_until_error(source.data(), source.data() + std::size(source), config);
    }

    [[nodiscard]] std::tuple<parser_result, std::vector<parser_error_report>>
    try_force_parse(const std::string_view source, const parser_config& config) noexcept
    {
        return details::_try_parse_buffer_skip_errors(source.data(), source.data() + std::size(source), config);
    }

} // namespace drako::file_formats::obj