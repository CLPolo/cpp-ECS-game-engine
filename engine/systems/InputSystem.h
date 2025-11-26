/**
 * @file InputSystem.h
 * @brief Processes key presses.
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include "../components/CollisionComponent.h"
#include "../components/InputComponent.h"
#include "../components/MovementComponent.h"
#include "../managers/EntityManager.h"
#include <SFML/Window/Keyboard.hpp>

#include <iostream>

namespace ECSEngine {

// Platformer physics constants
constexpr float PLAYER_AIR_CONTROL = 0.8f; // Air control multiplier
constexpr float JUMP_VELOCITY = -400.0f; // UP WE GO~!
constexpr float WALL_JUMP_VELOCITY_Y = -350.0f; // Wall jump UP WE GO~!
constexpr float WALL_JUMP_VELOCITY_X = 250.0f; // Wall jump
constexpr float FAST_FALL_SPEED = 200.0f; // Extra downward velocity when holding S
constexpr float WALL_SLIDE_GRAVITY_SCALE = 0.3f; // Gravity reduction when sliding on wall

/**
 * @brief This system processes any keys pressed and transforms them into the
 * velocities that will be used to update the entity location.
 *
 * @details Implements platformer physics with gradual acceleration, wall jumping,
 * and wall slide mechanics. Uses collision information from the last frame.
 *
 * @tparam Components The components for an entity.
 * @param entityManager Reference to the Entity Manager.
 * @param deltaTime Time elapsed since last frame (in seconds).
 */
template <typename... Components>
void InputSystem(
    EntityManager<Components...>& entityManager, SoundManager& soundManager, float deltaTime)
{
    for (auto entity : entityManager) {

        if (!entity.id || !entityManager.template HasComponent<InputComponent>(entity.id)
            || !entityManager.template HasComponent<MovementComponent>(entity.id)) {
            continue;
        }

        auto& input = entityManager.template GetComponent<InputComponent>(entity.id);
        auto& movement = entityManager.template GetComponent<MovementComponent>(entity.id);
        auto& acceleration
            = entityManager.template GetComponent<AccelerationComponent>(entity.id).acceleration;

        // Get collision state for platformer mechanics
        bool isGrounded = false;
        bool againstLeftWall = false;
        bool againstRightWall = false;
        bool isFalling = movement.velocity.y > 0;

        if (entityManager.template HasComponent<CollisionComponent>(entity.id)) {
            auto& collision = entityManager.template GetComponent<CollisionComponent>(entity.id);
            isGrounded = collision.collidedBottom;
            againstLeftWall = collision.collidedLeft;
            againstRightWall = collision.collidedRight;

            if (isFalling) {
                collision.wasStandingLast = false;
            }
        }

        // Wall slide state: pushing against wall while falling
        bool isWallSliding = isFalling
            && ((againstLeftWall && input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::A)))
                || (againstRightWall
                    && input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::D))));

        // Horizontal movement: gradual acceleration up to max speed
        float horizontalInput = 0.0f;
        if (input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::A))) {
            horizontalInput -= 1.0f;
        }
        if (input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::D))) {
            horizontalInput += 1.0f;
        }

        if (horizontalInput != 0.0f) {
            // Apply acceleration (reduced in air for better control)
            float accel = acceleration.x;
            if (!isGrounded) {
                accel *= PLAYER_AIR_CONTROL;
            }

            // Gradual velocity change
            float targetVelocity = horizontalInput * movement.maxSpeed;
            if (std::abs(targetVelocity - movement.velocity.x) < accel * deltaTime) {
                movement.velocity.x = targetVelocity;
            } else {
                movement.velocity.x
                    += (targetVelocity > movement.velocity.x ? accel : -accel) * deltaTime;
            }

            // Clamp to max speed
            if (std::abs(movement.velocity.x) > movement.maxSpeed) {
                movement.velocity.x = (movement.velocity.x > 0 ? 1.0f : -1.0f) * movement.maxSpeed;
            }
        } else {
            // Deceleration when no input
            float decel = isGrounded ? acceleration.x * 1.5f : acceleration.x * 0.5f;
            if (std::abs(movement.velocity.x) < decel * deltaTime) {
                movement.velocity.x = 0.0f;
            } else {
                movement.velocity.x -= (movement.velocity.x > 0 ? decel : -decel) * deltaTime;
            }
        }

        // Jumping mechanics
        bool jumpPressed = input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::W))
            || input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::Space));

        if (jumpPressed) {

            if (isGrounded) {
                // Ground jump
                movement.velocity.y = JUMP_VELOCITY;
                soundManager.PlaySound("jump");

            } else if (isWallSliding) {
                // Wall jump - push away from wall
                movement.velocity.y = WALL_JUMP_VELOCITY_Y;
                soundManager.PlaySound("jump");

                if (againstLeftWall) {
                    movement.velocity.x = WALL_JUMP_VELOCITY_X; // Push right
                } else if (againstRightWall) {
                    movement.velocity.x = -WALL_JUMP_VELOCITY_X; // Push left
                }
            }
        }

        // Fast fall
        if (input.keydown.test(static_cast<size_t>(sf::Keyboard::Scan::S))) {
            movement.velocity.y += FAST_FALL_SPEED;
        }
    }
}

} // namespace ECSEngine
