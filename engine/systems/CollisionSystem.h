/**
 * @file CollisionSystem.h
 * @brief Checks for and resolves collisions between entities.
 */

#pragma once

#include <cassert>
#include <cmath>
#include <vector>

#include "../components/CollisionComponent.h"
#include "../components/InputComponent.h"
#include "../components/LocationComponent.h"
#include "../components/MovementComponent.h"
#include "../core/MathUtil.h"
#include "../managers/EntityManager.h"

namespace ECSEngine {

/**
 * @brief Stores penetration depths between two overlapping rectangles.
 */
struct Overlap {
    float horizontal; // Penetration depth on X axis
    float vertical; // Penetration depth on Y axis
};

/**
 * @brief Calculates penetration depths between two bounding boxes.
 * @param rect1 First bounding box
 * @param rect2 Second bounding box
 * @return Overlap containing horizontal and vertical penetration depths
 */
inline Overlap CalculateOverlap(const Rect& rect1, const Rect& rect2)
{
    Overlap overlap;

    float left1 = rect1.topLeft.x;
    float right1 = left1 + rect1.width;
    float top1 = rect1.topLeft.y;
    float bottom1 = top1 + rect1.height;

    float left2 = rect2.topLeft.x;
    float right2 = left2 + rect2.width;
    float top2 = rect2.topLeft.y;
    float bottom2 = top2 + rect2.height;

    // Calculate actual penetration depth (intersection region)
    overlap.horizontal = std::min(right1, right2) - std::max(left1, left2);
    overlap.vertical = std::min(bottom1, bottom2) - std::max(top1, top2);

    return overlap;
}

/**
 * @brief Resolves collision between two entities by separating them.
 * @details Determines collision direction from previous positions, then pushes
 * dynamic entities out of static entities. Sets appropriate collision flags.
 *
 * @tparam Components The component types in the EntityManager
 * @param entity1 First entity ID
 * @param entity2 Second entity ID
 * @param col1 Collision component of first entity
 * @param col2 Collision component of second entity
 * @param overlap Overlap information between the two entities
 * @param entityManager Reference to the entity manager
 */
template <typename... Components>
inline void ResolveCollision(EntityID entity1, EntityID entity2, CollisionComponent& col1,
    CollisionComponent& col2, const Overlap& overlap, EntityManager<Components...>& entityManager,
    EntityID theCamera, EntityID thePlayer)
{
    // Check once at the top - used throughout function
    bool entity1IsPlayer = (entity1 == thePlayer);
    bool entity2IsPlayer = (entity2 == thePlayer);

    // Dynamic entities must have LocationComponent to be moved
    // note: Static-static pairs are filtered out before this function is called
    if (!col1.isStatic) {
        assert(entityManager.template HasComponent<LocationComponent>(entity1)
            && "Dynamic entity must have LocationComponent!");
    }
    if (!col2.isStatic) {
        assert(entityManager.template HasComponent<LocationComponent>(entity2)
            && "Dynamic entity must have LocationComponent!");
    }
    // note: we keep the HasComponent checks below for when compiling without debug

    const float bounceDamping = 0.7f; // TODO: should be in a component?

    // Resolve on axis with smallest overlap
    bool separateHorizontally = overlap.horizontal < overlap.vertical;

    // Handle dynamic-dynamic collisions
    if (!col1.isStatic && !col2.isStatic) {
        // Both entities dynamic - push both apart by half overlap
        if (!entityManager.template HasComponent<LocationComponent>(entity1)
            || !entityManager.template HasComponent<LocationComponent>(entity2))
            return;

        auto& location1 = entityManager.template GetComponent<LocationComponent>(entity1);
        auto& location2 = entityManager.template GetComponent<LocationComponent>(entity2);

        if (separateHorizontally) {
            float halfOverlap = overlap.horizontal / 2.0f;
            bool entity1IsLeft
                = col1.currentBoundingBox.topLeft.x < col2.currentBoundingBox.topLeft.x;

            if (entity1IsLeft) {
                location1.position.x -= halfOverlap;
                col1.currentBoundingBox.topLeft.x -= halfOverlap;
                location2.position.x += halfOverlap;
                col2.currentBoundingBox.topLeft.x += halfOverlap;
                col1.collidedRight = true;
                col2.collidedLeft = true;
            } else {
                location1.position.x += halfOverlap;
                col1.currentBoundingBox.topLeft.x += halfOverlap;
                location2.position.x -= halfOverlap;
                col2.currentBoundingBox.topLeft.x -= halfOverlap;
                col1.collidedLeft = true;
                col2.collidedRight = true;
            }

            // Exchange horizontal velocities with damping (non-player entities only)
            if (entityManager.template HasComponent<MovementComponent>(entity1)
                && entityManager.template HasComponent<MovementComponent>(entity2)) {
                auto& movement1 = entityManager.template GetComponent<MovementComponent>(entity1);
                auto& movement2 = entityManager.template GetComponent<MovementComponent>(entity2);

                // If both are non-players (e.g., two stars), exchange velocities
                if (!entity1IsPlayer && !entity2IsPlayer) {
                    float temp = movement1.velocity.x;
                    movement1.velocity.x = movement2.velocity.x * bounceDamping;
                    movement2.velocity.x = temp * bounceDamping;
                }
                // If only entity1 is a star, bounce it off the player
                else if (!entity1IsPlayer && entity2IsPlayer) {
                    movement1.velocity.x = -movement1.velocity.x * bounceDamping;
                }
                // If only entity2 is a star, bounce it off the player
                else if (entity1IsPlayer && !entity2IsPlayer) {
                    movement2.velocity.x = -movement2.velocity.x * bounceDamping;
                }
                // If both are players (shouldn't happen), don't bounce either
            }
        } else {
            float halfOverlap = overlap.vertical / 2.0f;
            bool entity1IsAbove
                = col1.currentBoundingBox.topLeft.y < col2.currentBoundingBox.topLeft.y;

            if (entity1IsAbove) {
                location1.position.y -= halfOverlap;
                col1.currentBoundingBox.topLeft.y -= halfOverlap;
                location2.position.y += halfOverlap;
                col2.currentBoundingBox.topLeft.y += halfOverlap;
                col1.collidedBottom = true;
                col2.collidedTop = true;
            } else {
                location1.position.y += halfOverlap;
                col1.currentBoundingBox.topLeft.y += halfOverlap;
                location2.position.y -= halfOverlap;
                col2.currentBoundingBox.topLeft.y -= halfOverlap;
                col1.collidedTop = true;
                col2.collidedBottom = true;
            }

            // Exchange vertical velocities with damping only for non-player entities
            bool entity1IsPlayer = entityManager.template HasComponent<InputComponent>(entity1);
            bool entity2IsPlayer = entityManager.template HasComponent<InputComponent>(entity2);

            if (entityManager.template HasComponent<MovementComponent>(entity1)
                && entityManager.template HasComponent<MovementComponent>(entity2)) {
                auto& movement1 = entityManager.template GetComponent<MovementComponent>(entity1);
                auto& movement2 = entityManager.template GetComponent<MovementComponent>(entity2);

                // If both are non-players (e.g., two stars), exchange velocities
                if (!entity1IsPlayer && !entity2IsPlayer) {
                    float temp = movement1.velocity.y;
                    movement1.velocity.y = movement2.velocity.y * bounceDamping;
                    movement2.velocity.y = temp * bounceDamping;

                    // Stop horizontal movement when both have settled
                    if (std::abs(movement1.velocity.y) < 10.0f
                        && std::abs(movement2.velocity.y) < 10.0f) {
                        movement1.velocity.x = 0.0f;
                        movement2.velocity.x = 0.0f;
                    }
                }
            }
        }
        return; // Early return after handling dynamic-dynamic
    }

    // Handle static-dynamic collisions (horizontal)
    if (separateHorizontally) {
        // Determine which entity is dynamic
        EntityID dynamicEntity = !col1.isStatic ? entity1 : entity2;
        CollisionComponent& dynamicCol = !col1.isStatic ? col1 : col2;
        CollisionComponent& staticCol = !col1.isStatic ? col2 : col1;

        // Determine direction: is the dynamic entity to the right of static?
        bool dynamicIsRight
            = dynamicCol.currentBoundingBox.topLeft.x > staticCol.currentBoundingBox.topLeft.x;

        // Set collision flags
        if (dynamicIsRight) {
            // Dynamic entity is to the right - push it right (away from static)
            dynamicCol.collidedLeft = true;
            staticCol.collidedRight = true;
        } else {
            // Dynamic entity is to the left - push it left (away from static)
            dynamicCol.collidedRight = true;
            staticCol.collidedLeft = true;
        }

        // Apply separation to dynamic entity
        if (entityManager.template HasComponent<LocationComponent>(dynamicEntity)) {
            auto& location = entityManager.template GetComponent<LocationComponent>(dynamicEntity);
            float separation = dynamicIsRight ? overlap.horizontal : -overlap.horizontal;

            location.position.x += separation;
            dynamicCol.currentBoundingBox.topLeft.x += separation;

            if (entityManager.template HasComponent<MovementComponent>(dynamicEntity)) {
                auto& movement
                    = entityManager.template GetComponent<MovementComponent>(dynamicEntity);

                if (dynamicEntity != thePlayer) {
                    // Reverse velocity if moving toward the wall
                    if ((dynamicIsRight && movement.velocity.x < 0)
                        || (!dynamicIsRight && movement.velocity.x > 0)) {
                        movement.velocity.x = -movement.velocity.x * bounceDamping;
                    }
                } else {
                    // Player-specific wall collision handling
                    // Stop accumulating horizontal velocity when shoving into walls
                    movement.velocity.x = 0.0f;

                    // Clamp downward speed for wall-slide behavior when pushing into wall
                    if (movement.velocity.y > 150.0f) {
                        movement.velocity.y = 150.0f;
                    }

                    // Trigger horizontal camera shake only on first contact
                    if (!dynamicCol.wasTouchingWallLast) {
                        auto& shake = entityManager.template GetComponent<CameraShake>(theCamera);
                        shake.isShaking = true;
                        shake.horizontal = true;
                    }
                }
            }
        }
    }
    // Handle static-dynamic collisions (vertical)
    else {
        // Determine which entity is dynamic
        EntityID dynamicEntity = !col1.isStatic ? entity1 : entity2;
        CollisionComponent& dynamicCol = !col1.isStatic ? col1 : col2;
        CollisionComponent& staticCol = !col1.isStatic ? col2 : col1;

        // Determine direction: is the dynamic entity positioned below the static?
        bool dynamicIsBelow
            = dynamicCol.currentBoundingBox.topLeft.y > staticCol.currentBoundingBox.topLeft.y;

        // Set collision flags
        if (dynamicIsBelow) {
            // Dynamic entity is below - push it down (away from static)
            dynamicCol.collidedTop = true;
            staticCol.collidedBottom = true;
        } else {
            // Dynamic entity is above - push it up (away from static, landing on platform)
            dynamicCol.collidedBottom = true;
            staticCol.collidedTop = true;
        }

        // Apply separation to dynamic entity
        if (entityManager.template HasComponent<LocationComponent>(dynamicEntity)) {
            auto& location = entityManager.template GetComponent<LocationComponent>(dynamicEntity);
            float separation = dynamicIsBelow ? overlap.vertical : -overlap.vertical;

            location.position.y += separation;
            dynamicCol.currentBoundingBox.topLeft.y += separation;

            if (entityManager.template HasComponent<MovementComponent>(dynamicEntity)) {
                auto& movement
                    = entityManager.template GetComponent<MovementComponent>(dynamicEntity);

                if (dynamicEntity != thePlayer) {
                    // Reverse velocity if moving toward the platform
                    if ((dynamicIsBelow && movement.velocity.y < 0)
                        || (!dynamicIsBelow && movement.velocity.y > 0)) {
                        movement.velocity.y = -movement.velocity.y * bounceDamping;
                    }

                    // Stop horizontal movement when settled on ground (landing from above)
                    if (!dynamicIsBelow && std::abs(movement.velocity.y) < 10.0f) {
                        movement.velocity.x = 0.0f;
                    }
                } else {
                    // Player landing or hitting ceiling – zero vertical velocity
                    if (!dynamicIsBelow && movement.velocity.y > 0.0f) {
                        movement.velocity.y = 0.0f;
                    } else if (dynamicIsBelow && movement.velocity.y < 0.0f) {
                        movement.velocity.y = 0.0f;
                    }

                    if (!dynamicCol.wasStandingLast) {
                        // Trigger vertical camera shake
                        auto& shake = entityManager.template GetComponent<CameraShake>(theCamera);
                        shake.isShaking = true;
                        shake.horizontal = false;
                    }
                }
            }
        }

    }
}

/**
 * @brief Checks for collisions between objects and resolves them.
 * @details Uses AABB (Axis-Aligned Bounding Box) collision detection.
 * Determines collision direction from overlap amounts and resolves by
 * pushing dynamic entities out of static entities. Sets appropriate collision
 * flags (collidedTop, collidedBottom, collidedLeft, collidedRight).
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 */
template <typename... Components>
void CollisionSystem(EntityManager<Components...>& entityManager, SoundManager& soundManager)
{
    // Clears flags, updates bounding boxes, collects entities, finds camera/player
    EntityID theCamera = 0;
    EntityID thePlayer = 0;
    std::vector<EntityID> entities;

    for (const auto& entity : entityManager) {
        if (!entity.id)
            continue;

        // Process entities with collision components
        if (entityManager.template HasComponent<CollisionComponent>(entity.id)) {
            auto& collision = entityManager.template GetComponent<CollisionComponent>(entity.id);

            // Clear collision flags from previous frame
            // Must happen AFTER GravitySystem has read them (timing fix)
            collision.clearCollisions();

            // Update bounding box if entity has location
            if (entityManager.template HasComponent<LocationComponent>(entity.id)) {
                // Skip bounding box update for static entities that are already initialized
                if (!collision.isStatic || !collision.boundingBoxInitialized) {
                    const auto& location = entityManager.template GetComponent<LocationComponent>(entity.id);
                    // Update bounding box position based on entity location + offset
                    collision.currentBoundingBox.topLeft = location.position + collision.boundingBoxOffset;
                    collision.boundingBoxInitialized = true;
                }
            } else {
                assert(false && "Entity with CollisionComponent must have LocationComponent!");
            }

            // Add to collision entities list
            entities.push_back(entity.id);
        }

        // Find camera entity (by name)
        if (entityManager.template GetEntityName(entity.id) == "main_camera") {
            theCamera = entity.id;
        }

        // Find player entity (by InputComponent)
        if (entityManager.template HasComponent<InputComponent>(entity.id)) {
            thePlayer = entity.id;
        }
    }

    // Check each pair for collisions
    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            EntityID entity1 = entities[i];
            EntityID entity2 = entities[j];

            // Only check for collisions against valid entities
            if (!entityManager.ValidEntity(entity1) || !entityManager.ValidEntity(entity2))
                continue;

            auto& collision1 = entityManager.template GetComponent<CollisionComponent>(entity1);
            auto& collision2 = entityManager.template GetComponent<CollisionComponent>(entity2);

            // Skip static-static pairs (they never interact)
            if (collision1.isStatic && collision2.isStatic)
                continue;

            // Check if bounding boxes intersect
            if (!collision1.currentBoundingBox.RectIntersect(collision2.currentBoundingBox))
                continue;

            // Collision detected!

            // Check for player-star collision (star collection)
            EntityID star = 0;
            if (entity1 == thePlayer && entityManager.template GetEntityName(entity2) == "star") {
                star = entity2;
            } else if (entity2 == thePlayer && entityManager.template GetEntityName(entity1) == "star") {
                star = entity1;
            }

            if (star != 0) {
                // Player collected a star
                auto& score = entityManager.template GetComponent<ScoreComponent>(thePlayer);
                auto& sprite = entityManager.template GetComponent<SpriteComponent>(star);

                score.score += 10;
                sprite.isAlive = false;
                entityManager.RemoveEntity(star);
                soundManager.PlaySound("sparkle");
                continue;
            }

            // Calculate overlap and resolve
            Overlap overlap
                = CalculateOverlap(collision1.currentBoundingBox, collision2.currentBoundingBox);

            ResolveCollision(
                entity1, entity2, collision1, collision2, overlap, entityManager, theCamera, thePlayer);
        }
    }

    // Update contact state for player after all collisions processed
    if (thePlayer && entityManager.template HasComponent<CollisionComponent>(thePlayer)) {
        auto& collision = entityManager.template GetComponent<CollisionComponent>(thePlayer);
        collision.wasTouchingWallLast = (collision.collidedLeft || collision.collidedRight);
        collision.wasStandingLast = collision.collidedBottom;
    }
}

} // namespace ECSEngine
