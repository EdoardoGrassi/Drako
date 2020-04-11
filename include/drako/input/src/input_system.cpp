#pragma once
#include "drako/input/input_system.hpp"

namespace drako::engine
{
    std::remove_const_t<device_id> input_system::_last_device = 0;

} // namespace drako::engine