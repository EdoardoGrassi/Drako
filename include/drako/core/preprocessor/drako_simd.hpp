#pragma once
#ifndef DRAKO_SIMD_HPP
#define DRAKO_SIMD_HPP

//
//  \brief      Provides symbols definitions for SIMD support.
//  \author     Grassi Edoardo
//


#include "drako/core/compiler.hpp"

namespace drako
{
    /* SSE */
    #if defined(DRAKO_CC_MSVC)

    // Defined if the compiler supports the SSE instructions set.
    #define DRKAPI_SIMD_SSE __SSE__

    #endif


    /* AVX */
    #if defined(DRAKO_CC_MSVC)

    // Defined if the compiler supports the AVX instructions set.
    #define DRKAPI_SIMD_AVX __AVX__

    #endif


    /* AVX2 */
    #if defined(DRAKO_CC_MSVC)

    #define DRKAPI_SIMD_AVX2 __AVX2__

    #endif


    /* AVX512 */
    #if defined(DRAKO_CC_MSVC)

    #define DRKAPI_SIMD_AVX512 __AVX2__

    #endif
}

#endif // !DRAKO_SIMD_HPP
