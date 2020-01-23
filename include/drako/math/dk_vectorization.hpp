#pragma once
#ifndef DRAKO_MATH_VECTORIZATION_HPP
#define DRAKO_MATH_VECTORIZATION_HPP

#include <intrin.h>

#include "drako/devel/dk_assertion.hpp"

#include "drako/math/dk_matrix4x4.hpp"

namespace drako
{
    void v_mul_kernel(const mat4x4* DRAKO_HPP_RESTRICT in,
        const mat4x4 kernel,
        const size_t count,
        mat4x4* DRAKO_HPP_RESTRICT out) noexcept
    {
        // NOTE: matrices needs to be stored in column-major form
        DRAKO_ASSERT(in != nullptr);
        DRAKO_ASSERT(out != nullptr);
        DRAKO_ASSERT(in != out);

        __m512 kernel_x = _mm512_permute_ps(kernel, 0b00000000);    // [x1 x1 x1 x1, y1 y1 y1 y1, z1 z1 z1 z1, w1 w1 w1 w1]
        __m512 kernel_y = _mm512_permute_ps(kernel, 0b01010101);    // [x2 x2 x2 x2, y2 y2 y2 y2, z2 z2 z2 z2, w2 w2 w2 w2]
        __m512 kernel_z = _mm512_permute_ps(kernel, 0b10101010);    // [x3 x3 x3 x3, y3 y3 y3 y3, z3 z3 z3 z3, w3 w3 w3 w3]
        __m512 kernel_w = _mm512_permute_ps(kernel, 0b11111111);    // [x4 x4 x4 x4, y4 y4 y4 y4, z4 z4 z4 z4, w4 w4 w4 w4]

        for (auto i = 0; i < count; i++)
        {
            out[i] = _mm512_mul_ps(in[i], kernel_x);
            out[i] = _mm512_fmadd_ps(in[i], kernel_y, out[i]);
            out[i] = _mm512_fmadd_ps(in[i], kernel_z, out[i]);
            out[i] = _mm512_fmadd_ps(in[i], kernel_w, out[i]);
        }
    }

    struct SoA_Quaternion
    {
        float* DRAKO_HPP_RESTRICT x;
        float* DRAKO_HPP_RESTRICT y;
        float* DRAKO_HPP_RESTRICT z;
        float* DRAKO_HPP_RESTRICT w;
    };


    void v_slerp(
        const SoA_Quaternion in_lhs,
        const SoA_Quaternion in_rhs,
        const SoA_Quaternion out,
        const size_t count) noexcept
    {
        // TODO: add assertions on input
    }
}

#endif // !DRAKO_MATH_VECTORIZATION_HPP_

