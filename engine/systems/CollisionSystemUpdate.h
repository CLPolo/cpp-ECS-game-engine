/**
 * @file CollisionSystemUpdate.h
 * @brief Handles frame updates for collisions.
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include "../managers/EntityManager.h"
#include "../components/CollisionComponent.h"

namespace ECSEngine {

/**
 * @brief Stores previous bounding boxes before movement is applied.
 *
 * @details This runs BEFORE GravitySystem and MovementSystem so we have
 * the pre-movement state stored. Collision flags are NOT cleared here
 * so GravitySystem can read them.
 *
 * @tparam Components The components for an entity.
 * @param entityManager Reference to the entity manager.
 */
template <typename... Components>
void CollisionSystemUpdate(EntityManager<Components...>& entityManager)
{
    for (const auto& entity : entityManager) {

        if (!entity.id || !entityManager.template HasComponent<CollisionComponent>(entity.id)) {
            continue;
        }

        auto& collision = entityManager.template GetComponent<CollisionComponent>(entity.id);

        // Store previous bounding box (skip for static entities after first frame)
        if (!collision.isStatic || !collision.boundingBoxInitialized) {
            collision.previousBoundingBox = collision.currentBoundingBox;
        }

        // NOTE: Collision flags are cleared in CollisionSystem AFTER GravitySystem
        // has had a chance to read them
    }
}

} // namespace ECSEngine
