#pragma once
#ifndef DRAKO_PARTICLE_HPP
#define DRAKO_PARTICLE_HPP

#include "drako/devel/assertion.hpp"
#include "drako/math/vector2.hpp"

namespace drako::pyx
{
    struct particle
    {
        vec2 position;
        vec2 velocity;
        vec2 acceleration;

        // Inverse of the particle mass
        float mass_inverse;
    };

    // Updates particle state forward in time with Newton-Euler linear integration.
    void newton_euler_integrate(particle& p, float dt) noexcept
    {
        DRAKO_PRECON(dt > 0.0f, "Time delta can't be negative or zero");

        // update position
        p.position += p.velocity * dt + p.acceleration * dt * dt * 0.5f;
        // update velocity
        p.velocity += p.acceleration * dt;
    }


    class particle_system
    {
    };

} // namespace drako::pyx

#endif // !DRAKO_PARTICLE_HPP
