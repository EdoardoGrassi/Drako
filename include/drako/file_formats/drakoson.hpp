#ifndef DRAKO_DRAKOSON_HPP
#define DRAKO_DRAKOSON_HPP

#include <iostream>
#include <string>
#include <unordered_map>

namespace drako::file_formats::drakoson
{
    using object = std::unordered_map<std::string, std::string>;

    object from_stream(std::istream&);

    void to_stream(const object&, std::ostream&);

} // namespace drako::file_formats::drakoson

#endif // !DRAKO_DRAKOSON_HPP