#pragma once
#ifndef DRAKO_UUID_ENGINE_HPP
#define DRAKO_UUID_ENGINE_HPP

#include "drako/devel/uuid.hpp"

#include <array>
#include <cstddef>

namespace drako
{
    /// @brief Generator for standard UUIDs version 1.
    /// 
    /// Time-based version as specified in [RFC 4122].
    /// Requires access to the MAC address of the current machine.
    /// 
    class UuidMacEngine
    {
        // TODO: end impl
    public:
        explicit UuidMacEngine();

        //UuidMacEngine(const UuidMacEngine&) = delete;
        //UuidMacEngine& operator=(const UuidMacEngine&) = delete;

        [[nodiscard]] Uuid operator()() const noexcept;

    private:
        std::uint16_t            _clock;
        std::array<std::byte, 6> _mac;
    };
} // namespace drako

#endif // !DRAKO_UUID_ENGINE_HPP