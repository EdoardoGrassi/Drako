#pragma once
#ifndef DRAKO_SHADER_HPP
#define DRAKO_SHADER_HPP

#include <vector>

namespace drako::graphics
{
    class ShaderResource
    {
    public:

    private:

        std::vector<uint8_t> _vertex_shader_code;

        std::vector<uint8_t> _fragment_shader_code;
    };
}

#endif // !DRAKO_SHADER_HPP
