/**
 * @file GravitySystem.h
 * @brief Applies acceleration of velocity to entities.
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "../managers/EntityManager.h"
#include "../components/MovementComponent.h"
#include "../components/CollisionComponent.h"
#include "../components/InputComponent.h"
#include <SFML/Window/Keyboard.hpp>

namespace ECSEngine {

constexpr float GRAVITY = 980.0f;
constexpr float WALL_SLIDE_GRAVITY_MULTIPLIER = 0.3f;  // Reduce gravity to 30% when wall sliding

/**
 * @brief This system applies an acceleration to the velocity for any entities
 * with both the gravity and movement components.
 *
 * @details The gravity applied depends on the current collisions. Gravity is reduced
 * when wall sliding (pushing against a wall while falling).
 *
 * @tparam Components Components The components for an entity.
 * @param entityManager Reference to Entity Manager.
 * @param deltaTime Time elapsed since last frame (in seconds).
 */
template <typename... Components>
void GravitySystem(EntityManager<Components...>& entityManager, float deltaTime)
{
    for (const auto& entity : entityManager) {

        // Entity needs MovementComponent for velocity
        if (!entity.id || !entityManager.template HasComponent<MovementComponent>(entity.id)) {
            continue;
        }

        auto& movement = entityManager.template GetComponent<MovementComponent>(entity.id);

        // Check if entity has collision component to detect ground and walls
        bool isGrounded = false;
        bool isWallSliding = false;

        if (entityManager.template HasComponent<CollisionComponent>(entity.id)) {
            const auto& collision = entityManager.template GetComponent<CollisionComponent>(entity.id);

            // Check if on the ground
            isGrounded = collision.collidedBottom;

            // Check for wall slide: against wall + falling + pushing into wall
            if (!isGrounded && movement.velocity.y > 0 &&
                entityManager.template HasComponent<InputComponent>(entity.id)) {

                const auto& input = entityManager.template GetComponent<InputComponent>(entity.id);

                // Wall slide if pushing left into left wall OR pushing right into right wall
                isWallSliding = (collision.collidedLeft && input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::A)))
                             || (collision.collidedRight && input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::D)));
            }
        }

        // Skip gravity if on ground
        if (isGrounded) {
            continue;
        }

        // Apply gravity (reduced if wall sliding)
        float gravityMultiplier = isWallSliding ? WALL_SLIDE_GRAVITY_MULTIPLIER : 1.0f;
        movement.velocity.y += GRAVITY * gravityMultiplier * deltaTime;
    }
}

} // namespace ECSEngine
