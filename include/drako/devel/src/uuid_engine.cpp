#include "drako/devel/uuid_engine.hpp"

#include "drako/devel/src/uuid_defs.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <random>

namespace drako
{
    uuid_v1_engine::uuid_v1_engine()
    {
        auto rng = std::random_device{};
        _clock   = rng();

        _mac = {}; // TODO: end impl
    }

    uuid uuid_v1_engine::operator()() noexcept
    {
        // from [RFC 4122 4.1.4 Timestamp]
        //
        // For UUID version 1, this is represented by Coordinated Universal Time(UTC)
        // as a count of 100 - nanosecond intervals since 00 : 00 : 00.00, 15 October 1582
        // (the date of Gregorian reform to the Christian calendar)

        std::tm reference{
            .tm_sec  = 0,
            .tm_min  = 0,
            .tm_hour = 0,
            .tm_mday = 15,         // day of the month [1-31]
            .tm_mon  = 9,          // months since January [0-11]
            .tm_year = 1582 - 1900 // years since 1900
        };
        const auto     ref       = std::mktime(&reference);
        const auto     old       = std::chrono::system_clock::from_time_t(ref);
        const auto     now       = std::chrono::system_clock::now();
        const uint64_t timestamp = (now - old).count();

        return _assemble(_version::rfc4122_v1, timestamp, _clock, _mac);
    }
} // namespace drako