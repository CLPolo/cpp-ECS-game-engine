/**
 * @file SpriteComponent.h
 * @brief Sprite rendering component for entities.
 * @version 0.1
 * @date 2025-11-05
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "../core/MathUtil.h"
#include "../managers/SpriteManager.h"

namespace ECSEngine {

/**
 * @struct SpriteComponent
 * @brief Stores the sprite ID associated with an entity, the bounds of the sprite location
 * relative to the entity location in the world, and whether the sprite is in world or screen space.
 *
 * @details This is only defined if an entity also has a LocationComponent.
 */
struct SpriteComponent {
    SpriteID spriteID; // Sprite from SpriteManager
    Rect spriteRect; // Bounds relative to entity location
    bool worldSpace; // False = screen space
    bool isAlive;

    SpriteComponent()
        : spriteID(0)
        , spriteRect()
        , worldSpace(true)
        , isAlive(true)
    {
    }

    SpriteComponent(SpriteID id, const Rect& rect, bool isWorldSpace = true, bool living = true)
        : spriteID(id)
        , spriteRect(rect)
        , worldSpace(isWorldSpace)
        , isAlive(living)
    {
    }
};

} // namespace ECSEngine