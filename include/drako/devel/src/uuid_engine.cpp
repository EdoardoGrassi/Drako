#include "drako/devel/uuid_engine.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/src/uuid_defs.hpp"

#if defined(DRAKO_PLT_WIN32)
//#include <Windows.h>

#include <WinSock2.h>
#include <iphlpapi.h>
#endif

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <random>

namespace drako
{
    uuid_v1_engine::uuid_v1_engine()
    {
        _clock = std::random_device{}();

#if defined(DRAKO_PLT_WIN32)
        _mac = {}; // TODO: end impl

        ULONG size{};
        if (const auto ec = ::GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size);
            ec != ERROR_BUFFER_OVERFLOW)
            [[unlikely]] throw std::runtime_error{ "Couldn't get a MAC address." };

        auto p = std::make_unique<IP_ADAPTER_ADDRESSES[]>(size);
        if (const auto ec = ::GetAdaptersAddresses(AF_UNSPEC, 0, NULL, p.get(), &size);
            ec != ERROR_SUCCESS || size == 0)
            [[unlikely]] throw std::runtime_error{ "Couldn't get a MAC address." };

        // the minimum size of a MAC address is 6 bytes so we should always be safe
        assert(p[0].PhysicalAddressLength >= std::size(_mac));
        for (auto i = 0; i < std::size(_mac); ++i)
            _mac[i] = std::byte{ p[0].PhysicalAddress[i] };
#endif
    }

    uuid uuid_v1_engine::operator()() noexcept
    {
        // [RFC 4122 4.1.4 Timestamp]
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