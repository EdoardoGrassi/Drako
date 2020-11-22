#include "drako/file_formats/dson.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

namespace drako::dson
{
    object::object(std::string_view s)
    {
        for (auto beg = 0, end = 0, i = 1; beg < std::size(s); beg = end + 1, ++i)
        {
            end = s.find_first_of('\n', beg);

            const std::string_view line{ std::data(s) + beg, std::data(s) + end };
            if (std::empty(line))
                continue;

            const auto lsep = line.find('=');
            const auto rsep = line.rfind('=');
            if (line[0] == '=')
                throw std::runtime_error{ "Malformed Dson at line " + std::to_string(i) };
            if (line[line.size() - 1] == '=')
                throw std::runtime_error{ "Malformed Dson at line " + std::to_string(i) };
            if (lsep != rsep)
                throw std::runtime_error{ "Malformed Dson at line " + std::to_string(i) };

            _map.insert_or_assign(
                std::string{ line.substr(0, lsep - 1) },
                std::string{ line.substr(rsep + 1) });
        }
    }

    void object::set(const std::string& key, const std::string& value)
    {
        _map.insert_or_assign(key, value);
    }

    [[nodiscard]] std::string object::get(const std::string& key)
    {
        return _map.at(key);
    }

    [[nodiscard]] std::string object::get_or_default(
        const std::string& key, const std::string& def)
    {
        if (const auto it = _map.find(key); it != std::end(_map))
            return it->second;
        else
            return def;
    }

    std::istream& operator>>(std::istream& is, object& dson)
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

            dson._map.insert_or_assign(line.substr(0, sepl - 1), line.substr(sepr + 1));
        }
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const object& dson)
    {
        for (const auto& pair : dson._map)
        {
            if (pair.first.find('=') != std::string::npos)
                throw std::invalid_argument{ "Key contains forbidden '=' character" };
            if (pair.second.find('=') != std::string::npos)
                throw std::invalid_argument{ "Value contains forbidden '=' character" };

            os << pair.first << '=' << pair.second << '\n';
        }
        return os;
    }

} // namespace drako::dson