/**
 * @file CameraFollowerComponent.h
 * @brief Handles entity tracking.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include "../core/MathUtil.h"
#include <cstddef>

namespace ECSEngine {

using EntityID = size_t;

/**
 * @struct CameraFollower
 * @brief Marks an entity to be tracked by the camera.
 * @details Allows selective following on X and/or Y axes. For example,
 * a platformer might set followX=true, followY=false for horizontal-only following
 * as per the Project 2, Part 1 spec.
 */
struct CameraFollower {
    EntityID entityToTrack; // The entity that the camera should follow
    bool followX; // Follow horizontal movement
    bool followY; // Follow vertical movement

    CameraFollower()
        : entityToTrack(0)
        , followX(true)
        , followY(true)
    {
    }

    CameraFollower(EntityID entity, bool followHorizontal = true, bool followVertical = true)
        : entityToTrack(entity)
        , followX(followHorizontal)
        , followY(followVertical)
    {
    }
};

} // namespace