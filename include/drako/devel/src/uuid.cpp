#include "drako/devel/uuid.hpp"

#include "drako/devel/src/uuid_defs.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <random>
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
    static_assert(sizeof(_uuid_byte_layout) == sizeof(uuid),
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


    void _to_hex_chars(std::byte value, char* dst) noexcept
    {
        const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

        dst[0] = HEX[std::to_integer<std::uint8_t>(value) >> 4];
        dst[1] = HEX[std::to_integer<std::uint8_t>(value) & 0x0f];
    }

    void _to_hex_chars(std::uint8_t value, char* dst) noexcept
    {
        const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

        dst[0] = HEX[value >> 4];
        dst[1] = HEX[value & 0x0f];
    }

    void _to_hex_chars(std::uint16_t value, char* dst) noexcept
    {
        const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        static_assert(sizeof(HEX) == 16);

        dst[0] = HEX[(value >> 12) & 0x000f];
        dst[1] = HEX[(value >> 8) & 0x000f];
        dst[2] = HEX[(value >> 4) & 0x000f];
        dst[3] = HEX[value & 0x000f];
    }

    void _to_hex_chars(std::uint32_t value, char* dst) noexcept
    {
        const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        static_assert(sizeof(HEX) == 16);

        const std::uint32_t MASK = 0xf;

        dst[0] = HEX[(value >> 28) & MASK];
        dst[1] = HEX[(value >> 24) & MASK];
        dst[2] = HEX[(value >> 20) & MASK];
        dst[3] = HEX[(value >> 16) & MASK];
        dst[4] = HEX[(value >> 12) & MASK];
        dst[5] = HEX[(value >> 8) & MASK];
        dst[6] = HEX[(value >> 4) & MASK];
        dst[7] = HEX[value & MASK];
    }

    // avoid implicit conversions of other integers types
    template <typename T>
    void _to_hex_chars(T, char*) = delete;


    [[nodiscard]] std::byte _from_hex_chars(unsigned char msb, unsigned char lsb) noexcept
    {
        // [ 0b 0011 0000 = 0 ... 0b 0011 1001 = 9 ]
        // [ 0b 0100 0001 = A ... 0b 0100 0110 = F ]
        // [ 0b 0110 0001 = a ... 0b 0110 0110 = f ]

        // extracts value 1 if [A..F] or [a..f], bit 0 if [0..9]
        const uint8_t hex_alpha_bit = 0b0100'0000;

        const uint8_t _msb = ((msb & hex_alpha_bit) >> 6) * 9 + (msb & 0x0f);
        const uint8_t _lsb = ((lsb & hex_alpha_bit) >> 6) * 9 + (lsb & 0x0f);

        return std::byte{ _msb << 4 } | std::byte{ _lsb };
    }

    std::istream& operator>>(std::istream& is, uuid& u)
    {
        return is.read(reinterpret_cast<char*>(&u), sizeof(uuid));
    }

    std::ostream& operator<<(std::ostream& os, const uuid& u)
    {
        return os.write(reinterpret_cast<const char*>(&u), sizeof(uuid));
    }

    std::string to_string(const uuid& u)
    { // uuid: {8 hex-digits} '-' {4 hex-digits} '-' {4 hex-digits} '-' {4 hex-digits} '-' {12 hex-digits}
        const char UUID_STRING_SEPARATOR = '-';

        // time-low "-"
        // time-mid "-"
        // time-high-and-version "-"
        // clock-seq-and-reserved clock-seq-low "-"
        // node
        std::string buffer(UUID_STRING_SIZE, '\0');

        // time-low = 8 hex-digits ;
        for (auto i = 0; i < 4; i += 2)
            _to_hex_chars(u._bytes[i], &buffer[i]);
        buffer[4] = UUID_STRING_SEPARATOR;

        // time-mid = 4 hex-digits ;
        _to_hex_chars(u._bytes[8], &buffer[5 /* bytes 5, 6 */]);
        _to_hex_chars(u._bytes[9], &buffer[7 /* bytes 7, 8 */]);
        buffer[9] = UUID_STRING_SEPARATOR;

        // time-high-and-version = 4 hex-digits ;
        _to_hex_chars(u._bytes[10], &buffer[10 /* bytes 10, 11 */]);
        _to_hex_chars(u._bytes[11], &buffer[12 /* bytes 12, 13 */]);
        buffer[14] = UUID_STRING_SEPARATOR;

        // clock-seq-and-reserved = 2 hex-digits ;
        _to_hex_chars(u._bytes[12], &buffer[15 /* bytes 15, 16 */]);
        // clock-seq-low = 2 hex-digits ;
        _to_hex_chars(u._bytes[13], &buffer[17 /* bytes 17, 18 */]);
        buffer[19] = UUID_STRING_SEPARATOR;

        // node = 12 hex-digits ;
        for (size_t i = 0; i < 12; i += 2)
            _to_hex_chars(u._bytes[i], &buffer[20 + i]);

        return buffer;
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

    [[nodiscard]] uuid uuid::parse(const std::string_view s)
    {
        // accepted canonical format: xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
        const char hypen = '-';

        std::array<std::byte, 16> bytes;
        if (std::size(s) != (32 + 4))
            throw std::invalid_argument{ "Argument is not a UUID." };
        if ((s[UUID_HYPEN_1_OFFSET] != hypen) || (s[UUID_HYPEN_2_OFFSET] != hypen) ||
            (s[UUID_HYPEN_3_OFFSET] != hypen) || (s[UUID_HYPEN_4_OFFSET] != hypen))
            throw std::invalid_argument{ "Argument is not a UUID." };

        for (size_t i = 0, j = 0; i < UUID_STRING_SIZE; i += 2, ++j)
        {
            if (s[i] == UUID_HYPEN_1_OFFSET || s[i] == UUID_HYPEN_2_OFFSET ||
                s[i] == UUID_HYPEN_3_OFFSET || s[i] == UUID_HYPEN_4_OFFSET) // skip '-' separator
                ++i;

            const auto most_significant_bits = static_cast<unsigned char>(s[i]);
            const auto less_significant_bits = static_cast<unsigned char>(s[i + 1]);
            if (!std::isxdigit(most_significant_bits) || !std::isxdigit(less_significant_bits))
                throw std::invalid_argument{ "Argument is not a UUID." };
            bytes[j] = _from_hex_chars(most_significant_bits, less_significant_bits);
        }

        return uuid{ bytes };
    }

    std::variant<uuid, std::error_code> try_parse(const std::string_view s) noexcept
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
        return uuid{ bytes };
    }

} // namespace drako