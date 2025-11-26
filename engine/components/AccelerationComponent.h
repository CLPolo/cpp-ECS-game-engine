#pragma once

#include "../core/MathUtil.h"

namespace ECSEngine {

/**
 * @struct AccelerationComponent
 * @brief Stores the acceleration acting on an entity each frame.
 * Can be used for gravity or any other constant acceleration.
 */
struct AccelerationComponent {
    Point2D acceleration;

    AccelerationComponent()
        : acceleration { 0.0f, 0.0f }
    {
    }
    AccelerationComponent(float ax, float ay)
        : acceleration { ax, ay }
    {
    }
    AccelerationComponent(const Point2D& accel)
        : acceleration(accel)
    {
    }
};

} // namespcae ECSEngine