#pragma once

#include "../core/MathUtil.h"

namespace ECSEngine {

/**
 * @struct MovementComponent
 * @brief Stores the velocity of an entity.
 */
struct MovementComponent {
    Point2D velocity;
    float maxSpeed;

    MovementComponent()
        : velocity { 0.0f, 0.0f }
        , maxSpeed(300.0f)
    {
    }

    MovementComponent(float vx, float vy, float maxSpeedValue = 300.0f)
        : velocity { vx, vy }
        , maxSpeed(maxSpeedValue)
    {
    }

    MovementComponent(const Point2D& vel, float maxSpeedValue = 300.0f)
        : velocity(vel)
        , maxSpeed(maxSpeedValue)
    {
    }
};

} // namespace ECSEngine
