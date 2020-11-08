#include "drako/file_formats/dson.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

namespace drako::file_formats::dson
{
    void object::set(const std::string& key, const std::string& value) noexcept
    {
        _map.insert_or_assign(key, value);
    }

    [[nodiscard]] std::string object::get(const std::string& key)
    {
        return _map.at(key);
    }

    [[nodiscard]] std::string object::get_or_default(
        const std::string& key, const std::string& def) noexcept
    {
        if (const auto it = _map.find(key); it != std::end(_map))
            return it->second;
        else
            return def;
    }

    std::istream& operator>>(std::istream& is, object& dson)
    {
        for (auto i = 0;; ++i)
        {
            std::string line;
            std::getline(is, line);

            const auto sepl = line.find('=');
            const auto sepr = line.rfind('=');

            if (line[0] == '=')
                throw std::runtime_error{ "Malformed input file at line " + std::to_string(i) };
            if (line[line.size() - 1] == '=')
                throw std::runtime_error{ "Malformed input file at line " + std::to_string(i) };
            if (sepl != sepr)
                throw std::runtime_error{ "Malformed input file at line " + std::to_string(i) };

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

} // namespace drako::file_formats::dson