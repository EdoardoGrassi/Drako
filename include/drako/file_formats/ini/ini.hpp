#pragma once
#ifndef DRAKO_INI_HPP
#define DRAKO_INI_HPP

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace drako::ini
{
    /// @brief Document Object Model (DOM) instance.
    class Document
    {
    public:
        using Section = std::unordered_map<std::string, std::string>;

        explicit Document() = default;

        Document(const Document&) = default;
        Document& operator=(const Document&) = default;

        Document(Document&&) = default;
        Document& operator=(Document&&) = default;

        void set_section(std::string_view label, const Section& s);

        [[nodiscard]] const Section& get_section(std::string_view label) const;
        [[nodiscard]] Section&       get_section(std::string_view label);

    private:
        std::vector<std::string> _labels;
        std::vector<Section>     _values;
    };

    inline Document::Section& section(std::string_view label)
    {
        assert(!std::empty(label));

        const auto it = std::find(std::cbegin(_labels), std::end(_labels), s);
        if (it == std::cend(_labels))
            throw std::runtime_error{ "cannot find section " + s };

        return _values[std::distance(std::begin(_labels), it)];
    }

    std::istream& operator>>(std::istream&, Document&);
    std::ostream& operator<<(std::ostream&, Document&);

} // namespace drako::ini

#endif // !DRAKO_INI_HPP