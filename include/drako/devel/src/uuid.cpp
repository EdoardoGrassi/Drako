#include "drako/devel/uuid.hpp"

#include "drako/devel/src/uuid_defs.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <random>
#include <span>
#include <string>
#include <variant>

namespace drako
{
    // memory layout of canonical (network byte order) byte representation for UUIDs
    struct _uuid_byte_layout
    {
        std::uint64_t _time;
        std::uint16_t _clock;
        std::uint8_t  _node[6];
    };
    static_assert(sizeof(_uuid_byte_layout) == sizeof(Uuid),
        "Size mismatch between reference layout and metadata format");
    static_assert(std::is_standard_layout_v<_uuid_byte_layout>,
        "Required for correct behaviour of offsetof() macro (standard C++17)");


    // memory layout of canonical (network byte order) string representation for UUIDs
    struct _uuid_string_layout
    {
        char _time_low[8];
        char _hypen_1;
        char _time_mid[4];
        char _hypen_2;
        char _time_high_and_ver[4];
        char _hypen_3;
        char _clock[4];
        char _hypen_4;
        char _node[12];
    };
    static_assert(sizeof(_uuid_string_layout) == (sizeof(char) * 32 + sizeof('-') * 4),
        "Size mismatch between reference layout and metadata format");
    static_assert(std::is_standard_layout_v<_uuid_string_layout>,
        "Required for correct behaviour of offsetof() macro (standard C++17)");


    void _to_hex_chars(std::span<const std::byte> src, std::span<char> dst) noexcept
    {
        assert(std::data(src));
        assert(std::data(dst));
        assert(std::size(src) == 2 * std::size(dst));

        const char hex[] = "0123456789abcdef";
        static_assert(sizeof(hex) == 16 + 1);

        for (std::size_t i = 0; i < std::size(src); ++i)
        {
            dst[2 * i]     = hex[std::to_integer<uint8_t>(src[i] >> 4)];
            dst[2 * i + 1] = hex[std::to_integer<uint8_t>(src[i + 1]) & 0x0f];
        }
    }


    [[nodiscard]] std::byte _from_hex_chars(unsigned char msb, unsigned char lsb) noexcept
    {
        // [ 0b 0011 0000 = 0 ... 0b 0011 1001 = 9 ]
        // [ 0b 0100 0001 = A ... 0b 0100 0110 = F ]
        // [ 0b 0110 0001 = a ... 0b 0110 0110 = f ]

        // extracts value 1 if [A..F] or [a..f], bit 0 if [0..9]
        const std::uint8_t hex_alpha_bit{ 0b0100'0000 };

        const std::uint8_t _m{ msb };
        const std::uint8_t _l{ lsb };

        const std::uint8_t _msb = ((_m & hex_alpha_bit) >> 6) * 9 + (msb & 0x0f);
        const std::uint8_t _lsb = ((_l & hex_alpha_bit) >> 6) * 9 + (lsb & 0x0f);

        return static_cast<std::byte>((_msb << 4) | _lsb);
    };

    std::istream& operator>>(std::istream& is, Uuid& u)
    {
        return is.read(reinterpret_cast<char*>(std::data(u)), sizeof(Uuid));
    }

    std::ostream& operator<<(std::ostream& os, const Uuid& u)
    {
        return os.write(reinterpret_cast<const char*>(std::data(u)), sizeof(Uuid));
    }

    std::string to_string(const Uuid& u)
    { // uuid: {8 hex-digits} '-' {4 hex-digits} '-' {4 hex-digits} '-' {4 hex-digits} '-' {12 hex-digits}
        const char UUID_STRING_SEPARATOR = '-';

        // time-low "-"
        // time-mid "-"
        // time-high-and-version "-"
        // clock-seq-and-reserved clock-seq-low "-"
        // node
        std::string s(UUID_STRING_SIZE, '\0');

        // time-low = 8 hex-digits ;
        _to_hex_chars({ std::data(u), 4 }, { std::data(s), 8 });
        s[UUID_HYPEN_1_OFFSET] = UUID_STRING_SEPARATOR;

        // time-mid = 4 hex-digits ;
        _to_hex_chars({ std::data(u) + 4, 2 }, { std::data(s) + 8 + 1, 4 });
        s[UUID_HYPEN_2_OFFSET] = UUID_STRING_SEPARATOR;

        // time-high-and-version = 4 hex-digits ;
        _to_hex_chars({ std::data(u) + 6, 2 }, { std::data(s) + 12 + 2, 4 });
        s[UUID_HYPEN_3_OFFSET] = UUID_STRING_SEPARATOR;

        // clock-seq-and-reserved = 2 hex-digits ;
        // clock-seq-low = 2 hex-digits ;
        _to_hex_chars({ std::data(u) + 8, 2 }, { std::data(s) + 16 + 3, 4 });
        s[UUID_HYPEN_3_OFFSET] = UUID_STRING_SEPARATOR;

        // node = 12 hex-digits ;
        _to_hex_chars({ std::data(u) + 10, 6 }, { std::data(s) + 20 + 4, 12 });

        return s;
    }

    [[nodiscard]] std::uint16_t _init_clock_sequence() noexcept
    {
        using _rng         = std::mt19937;
        using _rng_adaptor = std::independent_bits_engine<_rng, sizeof(uint16_t), uint16_t>;

        const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        auto       rng  = _rng_adaptor(seed);
        return rng();
    }

    [[nodiscard]] auto _init_node_sequence()
    {
        using _rng      = std::mt19937_64;
        using _rng_mask = std::independent_bits_engine<_rng, 48, uint64_t>;

        std::random_device seeder;
        _rng_mask          rng(seeder());
        return rng();
    }

    void parse(const std::string_view s, Uuid& out)
    {
        // accepted canonical format: xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
        const char hypen = '-';

        std::array<std::byte, 16> bytes;
        if (std::size(s) != (32 + 4))
            throw std::invalid_argument{ "Not a valid UUID." };
        if ((s[UUID_HYPEN_1_OFFSET] != hypen) || (s[UUID_HYPEN_2_OFFSET] != hypen) ||
            (s[UUID_HYPEN_3_OFFSET] != hypen) || (s[UUID_HYPEN_4_OFFSET] != hypen))
            throw std::invalid_argument{ "Not a valid UUID." };

        for (size_t i = 0, j = 0; i < UUID_STRING_SIZE; i += 2, ++j)
        {
            if (s[i] == UUID_HYPEN_1_OFFSET || s[i] == UUID_HYPEN_2_OFFSET ||
                s[i] == UUID_HYPEN_3_OFFSET || s[i] == UUID_HYPEN_4_OFFSET) // skip '-' separator
                ++i;

            const auto most_significant_bits = static_cast<unsigned char>(s[i]);
            const auto less_significant_bits = static_cast<unsigned char>(s[i + 1]);
            if (!std::isxdigit(most_significant_bits) || !std::isxdigit(less_significant_bits))
                throw std::invalid_argument{ "Not a valid UUID." };
            bytes[j] = _from_hex_chars(most_significant_bits, less_significant_bits);
        }

        out = Uuid{ bytes };
    }

    std::variant<Uuid, std::error_code> try_parse(const std::string_view s) noexcept
    {
        // accepted canonical format: xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx

        const char HYPEN = '-';

        if (std::size(s) != UUID_STRING_SIZE) // 32 hex digits and 4 hypens
            return std::make_error_code(std::errc::invalid_argument);
        if ((s[UUID_HYPEN_1_OFFSET] != HYPEN) || (s[UUID_HYPEN_2_OFFSET] != HYPEN) ||
            (s[UUID_HYPEN_3_OFFSET] != HYPEN) || (s[UUID_HYPEN_4_OFFSET] != HYPEN))
            return std::make_error_code(std::errc::invalid_argument);

        std::array<std::byte, sizeof(_uuid_byte_layout)> bytes;
        for (size_t i = 0, j = 0; i < UUID_STRING_SIZE; i += 2, ++j)
        {
            if (s[i] == UUID_HYPEN_1_OFFSET || s[i] == UUID_HYPEN_2_OFFSET ||
                s[i] == UUID_HYPEN_3_OFFSET || s[i] == UUID_HYPEN_4_OFFSET) // skip '-' separator
                ++i;

            const auto most_significant_bits = static_cast<unsigned char>(s[i]);
            const auto less_significant_bits = static_cast<unsigned char>(s[i + 1]);
            if (!std::isxdigit(most_significant_bits) || !std::isxdigit(less_significant_bits))
                return std::make_error_code(std::errc::invalid_argument);

            bytes[j] = _from_hex_chars(most_significant_bits, less_significant_bits);
        }
        return Uuid{ bytes };
    }

} // namespace drako