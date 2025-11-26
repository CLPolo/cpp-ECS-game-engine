/**
 * @file CollisionComponent.h
 * @brief Collision detection and resolution component.
 */

#pragma once

#include "../core/MathUtil.h"

namespace ECSEngine {

/**
 * @struct CollisionComponent
 * @brief Tracks collision state and bounding boxes for collision detection.
 * @details Keeps track of which sides were in collision during the previous frame,
 * current and previous bounding boxes, and whether the entity is static or dynamic.
 * The boundingBoxOffset is relative to the entity's location and should not be modified.
 */
struct CollisionComponent {
    // Collision sides from previous frame
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;

    // For Shaking Purposes: was the player standing on something last frame
    bool wasStandingLast = true;

    // For wall shake: was the player touching a wall last frame (left or right)
    bool wasTouchingWallLast = false;

    // Offset from entity's position to bounding box top-left (do not modify after initialization)
    Point2D boundingBoxOffset;

    // Absolute bounding boxes in world space (updated by CollisionSystem each frame)
    // Note: width and height are constant; only topLeft changes per frame
    Rect currentBoundingBox;
    Rect previousBoundingBox;

    // Entity type for collision resolution
    // true = static (walls/platforms), false = dynamic (player/stars)
    bool isStatic;

    // Track if absolute bounding box has been initialized from location + offset
    bool boundingBoxInitialized;

    CollisionComponent()
        : collidedTop(false)
        , collidedBottom(false)
        , collidedLeft(false)
        , collidedRight(false)
        , boundingBoxOffset()
        , currentBoundingBox()
        , previousBoundingBox()
        , isStatic(true)
        , boundingBoxInitialized(false)
    {
    }

    CollisionComponent(const Rect& boundingBox, bool staticEntity = true)
        : collidedTop(false)
        , collidedBottom(false)
        , collidedLeft(false)
        , collidedRight(false)
        , boundingBoxOffset(boundingBox.topLeft) // Extract just the offset point
        , currentBoundingBox(
              boundingBox) // Width/height initialized here, topLeft updated by CollisionSystem
        , previousBoundingBox(boundingBox)
        , isStatic(staticEntity)
        , boundingBoxInitialized(false) // World position not yet computed
    {
    }

    // Helper to clear collision flags
    void clearCollisions()
    {
        collidedTop = false;
        collidedBottom = false;
        collidedLeft = false;
        collidedRight = false;
    }
};

}
