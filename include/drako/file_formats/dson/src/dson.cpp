#include "drako/file_formats/dson/dson.hpp"

#include "drako/file_formats/dson/src/dson_lexer.hpp"
#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace drako::dson
{
    namespace _fs = std::filesystem;

    [[nodiscard]] bool _dson_meta_char(char c) noexcept
    {
        const char meta[] = { '/', '=' };
        return std::find(std::cbegin(meta), std::end(meta), c) != std::cend(meta);
    }

    void _output_with_escaping() noexcept
    {
    }

    void DOM::set(const std::string& key, const std::string& val)
    {
        for (std::size_t i = 0; i < std::size(_keys); ++i)
            if (_keys[i] == key)
            {
                _vals[i] = val;
                return;
            }

        _keys.emplace_back(key);
        _vals.emplace_back(val);
    }

    [[nodiscard]] std::string DOM::get(const std::string& key) const
    {
        for (std::size_t i = 0; i < std::size(_keys); ++i)
            if (_keys[i] == key)
                return _vals[i];
        throw std::runtime_error{ "Key " + key + " not found." };
    }

    [[nodiscard]] std::string DOM::get_or_default(
        const std::string& key, const std::string& def) const noexcept
    {
        for (std::size_t i = 0; i < std::size(_keys); ++i)
            if (_keys[i] == key)
                return _vals[i];
        return def;
    }

    std::istream& operator>>(std::istream& is, DOM& dson)
    {
        for (auto i = 0; is; ++i)
        {
            std::string line;
            std::getline(is, line);

            const auto sepl = line.find('=');
            const auto sepr = line.rfind('=');

            if (line[0] == '=')
                throw std::runtime_error{ "Malformed input at line " + std::to_string(i) };
            if (line[line.size() - 1] == '=')
                throw std::runtime_error{ "Malformed input at line " + std::to_string(i) };
            if (sepl != sepr)
                throw std::runtime_error{ "Malformed input at line " + std::to_string(i) };

            dson.set(line.substr(0, sepl - 1), line.substr(sepr + 1));
        }
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const DOM& dson)
    {
        for (const auto& [key, val] : dson)
        {
            if (key.find('=') != std::string::npos)
                throw std::invalid_argument{ "Key contains forbidden '=' character" };
            if (val.find('=') != std::string::npos)
                throw std::invalid_argument{ "Value contains forbidden '=' character" };

            os << key << '=' << val << '\n';
        }
        return os;
    }

    [[nodiscard]] DOM parse(std::span<const char> source)
    {
        using TT = detail::Token::Type;
        DOM dom{};

        const auto tokens = detail::lex(source);
        for (auto t = std::cbegin(tokens); t != std::cend(tokens);)
        {
            if ((*t).type == TT::terminator)
            {
                ++t;
                continue;
            }

            if (std::distance(t, std::cend(tokens)) >= 4 &&
                t[0].type == TT::identifier &&
                t[1].type == TT::separator &&
                t[2].type == TT::identifier &&
                t[3].type == TT::terminator)
            {
                dom.set(t[0].view(), t[2].view());
                t += 4;
                continue;
            }
            throw std::runtime_error{ "Parsing error: unexpected token." };
        }
        return dom;
    }

    [[nodiscard]] std::string to_string(const DOM& dom)
    {
        // output formatted as key=value\n
        std::size_t chars = 0;
        for (const auto& [key, val] : dom) // precompute total size
            chars += std::size(key) + std::size(val) + 2;

        std::string s{};
        s.reserve(chars);
        for (const auto& [key, val] : dom)
        {
            s += key;
            s += '=';
            s += val;
            s += '\n';
        }
        return s;
    }

    void save(const _fs::path& file, const DOM& dom)
    {
        std::size_t chars = 0;
        for (const auto& key : dom._keys)
            chars += std::size(key);

        for (const auto& val : dom._vals)
            chars += std::size(val);

        auto  buffer = std::make_unique<char[]>(chars);
        char* end    = buffer.get();
        for (const auto& [key, val] : dom)
        {
            for (const auto c : key)
            {
                if (_dson_meta_char(c))
                    *(end++) = '\\'; // escape metachars
                *(end++) = c;
            }
            *(end++) = '=';
            for (const auto c : val)
            {
                if (_dson_meta_char(c))
                    *(end++) = '\\';
                *(end++) = c;
            }
            *(end++) = '\n';
        }

        io::UniqueOutputFile f{ file, io::Create::require_new };
        io::write_all(f, { reinterpret_cast<std::byte*>(buffer.get()), chars });
    }

} // namespace drako::dson