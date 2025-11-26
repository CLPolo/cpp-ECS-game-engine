/**
 * @file LocationComponent.h
 * @brief Position, velocity, and acceleration components for entity movement.
 */

#pragma once

#include "../core/MathUtil.h"

namespace ECSEngine {

/**
 * @struct LocationComponent
 * @brief Stores the base location of an entity in world coordinates.
 * All other locations or bounding boxes are stored relative to this point.
 */
struct LocationComponent {
    Point2D position;

    LocationComponent()
        : position { 0.0f, 0.0f }
    {
    }
    LocationComponent(float x, float y)
        : position { x, y }
    {
    }
    LocationComponent(const Point2D& pos)
        : position(pos)
    {
    }
};

}
