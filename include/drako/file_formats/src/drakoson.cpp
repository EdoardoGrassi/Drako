#include "drako/file_formats/drakoson.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

namespace drako::file_formats::drakoson
{
    object from_stream(std::istream& is)
    {
        object result;
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

            result.insert_or_assign(line.substr(0, sepl - 1), line.substr(sepr + 1));
        }
        return result;
    }

    void to_stream(const object& obj, std::ostream& os)
    {
        for (const auto& pair : obj)
        {
            if (pair.first.find('=') != std::string::npos)
                throw std::invalid_argument{ "Key contains forbidden '=' character" };
            if (pair.second.find('=') != std::string::npos)
                throw std::invalid_argument{ "Value contains forbidden '=' character" };

            os << pair.first << '=' << pair.second << '\n';
        }
    }

} // namespace drako::file_formats::drakoson