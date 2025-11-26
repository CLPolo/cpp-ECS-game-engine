/**
 * @file MovementSystem.h
 * @brief This system calculates the new location of an entity given
 * the entity velocity.
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include "../managers/EntityManager.h"
#include "../components/LocationComponent.h"
#include "../components/MovementComponent.h"

namespace ECSEngine {

/**
 * @brief This system calculates the new location of an entity given
 * the entity velocity.
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
template <typename... Components>
void MovementSystem(EntityManager<Components...>& entityManager, float deltaTime)
{
    for (const auto& entity : entityManager) {

        if (!entity.id || !entityManager.template HasComponent<LocationComponent>(entity.id)
            || !entityManager.template HasComponent<MovementComponent>(entity.id)) {
            continue;
        }

        auto& location = entityManager.template GetComponent<LocationComponent>(entity.id);
        const auto& movement = entityManager.template GetComponent<MovementComponent>(entity.id);

        // Update position based on velocity
        location.position.x += movement.velocity.x * deltaTime;
        location.position.y += movement.velocity.y * deltaTime;
    }
}

} // namespace ECSEngine
