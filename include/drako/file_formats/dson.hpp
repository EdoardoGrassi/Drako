#pragma once
#ifndef DRAKO_DSON_HPP
#define DRAKO_DSON_HPP

#include <iosfwd>
#include <string>
#include <unordered_map>

namespace drako::dson
{
    class object
    {
    public:
        explicit object() = default;

        explicit object(std::string_view);

        object(const object&) = default;
        object& operator=(const object&) = default;

        //std::string& operator[](const std::string_view key);

        [[nodiscard]] std::string get(const std::string& key);
        [[nodiscard]] std::string get_or_default(const std::string& key, const std::string& def);

        void set(const std::string& key, const std::string& value);

        friend std::string to_string(const object&);

        friend std::istream& operator>>(std::istream&, object&);
        friend std::ostream& operator<<(std::ostream&, const object&);

    private:
        std::unordered_map<std::string, std::string> _map;
    };

} // namespace drako::dson

#endif // !DRAKO_DSON_HPP