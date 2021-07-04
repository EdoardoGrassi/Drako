#pragma once
#ifndef DRAKO_DSON_HPP
#define DRAKO_DSON_HPP

#include <filesystem>
#include <iosfwd>
#include <iterator>
#include <span>
#include <string>
#include <tuple> // std::pair
#include <vector>

namespace drako::dson
{
    /// @brief Document Object Model instance.
    class DOM
    {
    public:
        class iterator;
        class const_iterator;

        explicit DOM() = default;

        explicit DOM(std::string_view);

        DOM(const DOM&) = default;
        DOM& operator=(const DOM&) = default;

        [[nodiscard]] std::string get(const std::string& key) const;
        [[nodiscard]] std::string get_or_default(
            const std::string& key, const std::string& def) const noexcept;

        void set(const std::string& key, const std::string& value);

        [[nodiscard]] iterator       begin() noexcept;
        [[nodiscard]] const_iterator begin() const noexcept;
        [[nodiscard]] const_iterator cbegin() const noexcept;

        [[nodiscard]] iterator       end() noexcept;
        [[nodiscard]] const_iterator end() const noexcept;
        [[nodiscard]] const_iterator cend() const noexcept;

        friend std::istream& operator>>(std::istream&, DOM&);
        friend std::ostream& operator<<(std::ostream&, const DOM&);

    private:
        // TODO: maybe use a single allocation for both buffers
        std::vector<std::string> _keys;
        std::vector<std::string> _vals;

    public:
        class iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = std::pair<const std::string&, std::string&>;

            iterator() noexcept = default;

            iterator(std::string* keys, std::string* vals) noexcept
                : _keys{ keys }, _vals{ vals } {}

            iterator& operator++() noexcept
            {
                ++_keys;
                ++_vals;
                return *this;
            }

            iterator operator++(int) noexcept
            {
                auto temp = *this;
                ++(*this);
                return temp;
            }

            iterator& operator--() noexcept
            {
                --_keys;
                --_vals;
                return *this;
            }

            iterator operator--(int) noexcept
            {
                auto temp = *this;
                --(*this);
                return temp;
            }

            [[nodiscard]] value_type operator*() const noexcept
            {
                return { *_keys, *_vals };
            }

            [[nodiscard]] bool operator==(const iterator&) const noexcept = default;
            [[nodiscard]] bool operator!=(const iterator&) const noexcept = default;

        private:
            std::string* _keys;
            std::string* _vals;
        };

        class const_iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = std::pair<const std::string&, const std::string&>;

            const_iterator() noexcept = default;

            const_iterator(const std::string* keys, const std::string* vals) noexcept
                : _keys{ keys }, _vals{ vals } {}

            const_iterator& operator++() noexcept
            {
                ++_keys;
                ++_vals;
                return *this;
            }

            const_iterator operator++(int) noexcept
            {
                auto temp = *this;
                ++(*this);
                return temp;
            }

            const_iterator& operator--() noexcept
            {
                --_keys;
                --_vals;
                return *this;
            }

            const_iterator operator--(int) noexcept
            {
                auto temp = *this;
                --(*this);
                return temp;
            }

            [[nodiscard]] value_type operator*() const noexcept
            {
                return { *_keys, *_vals };
            }

            [[nodiscard]] bool operator==(const const_iterator&) const noexcept = default;
            [[nodiscard]] bool operator!=(const const_iterator&) const noexcept = default;

        private:
            const std::string* _keys;
            const std::string* _vals;
        };
    };

    [[nodiscard]] inline DOM::iterator DOM::begin() noexcept
    {
        return { std::data(_keys), std::data(_vals) };
    }

    [[nodiscard]] inline DOM::const_iterator DOM::begin() const noexcept
    {
        return cbegin();
    }

    [[nodiscard]] inline DOM::const_iterator DOM::cbegin() const noexcept
    {
        return { std::data(_keys), std::data(_vals) };
    }

    [[nodiscard]] inline DOM::iterator DOM::end() noexcept
    {
        return { std::data(_keys) + std::size(_keys), std::data(_vals) + std::size(_vals) };
    }

    [[nodiscard]] inline DOM::const_iterator DOM::end() const noexcept
    {
        return cend();
    }

    [[nodiscard]] inline DOM::const_iterator DOM::cend() const noexcept
    {
        return { std::data(_keys) + std::size(_keys), std::data(_vals) + std::size(_vals) };
    }


    [[nodiscard]] DOM parse(std::span<const char> text);

    //[[nodiscard]] inline std::string to_string(const DOM&);

} // namespace drako::dson

#endif // !DRAKO_DSON_HPP