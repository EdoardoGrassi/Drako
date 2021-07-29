#include "drako/input/input_types.hpp"

#include <yaml-cpp/yaml.h>

namespace input
{
    //dson::DOM& operator>>(const dson::DOM& dom, ActionMap& a);

    //dson::DOM& operator<<(dson::DOM& dom, const ActionMap& a);

    const YAML::Node& operator>>(const YAML::Node& n, ActionMap& a)
    {
        throw std::runtime_error{ "Not implemented" };
    }

    YAML::Node& operator<<(YAML::Node& n, const ActionMap& a)
    {
        throw std::runtime_error{ "Not implemented" };
    }

} // namespace input