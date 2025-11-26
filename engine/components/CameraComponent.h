/**
 * @file CameraComponent.h
 * @brief Camera-related components for the ECS engine.
 */

#pragma once

#include "../core/MathUtil.h"
#include <cstddef>

namespace ECSEngine {

using EntityID = size_t;

/**
 * @struct CameraComponent
 * @brief Keeps track of the position and scale of the camera in world coordinates.
 */
struct CameraComponent {
    Point2D position;
    float worldUnitsPerPixel;
};

} // namespace ECSEngine
