#pragma once
#ifndef DRAKO_INPUT_DEVICE_HPP
#define DRAKO_INPUT_DEVICE_HPP

#include <cstdint>

#include "drako/core/meta/dk_serialization.hpp"
#include "drako/input/input_port.hpp"

namespace drako::engine
{
    struct virtual_unidir_axis
    {
    };

    struct virtual_bidir_axis
    {
    };

    struct virtual_button
    {
    };

} // namespace drako::engine

#endif // !DRAKO_INPUT_DEVICE_HPP