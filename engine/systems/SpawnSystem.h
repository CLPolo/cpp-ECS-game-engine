/**
 * @file SpawnSystem.h
 * @brief Handles entity spawning each frame.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <cassert>
#include <random>

#include "../components/CollisionComponent.h"
#include "../components/LocationComponent.h"
#include "../components/MovementComponent.h"
#include "../components/SpawnComponent.h"
#include "../components/SpriteComponent.h"
#include "../managers/EntityManager.h"
#include "../managers/SpriteManager.h"

#include <SFML/Graphics/Sprite.hpp>

namespace ECSEngine {

/**
 * @brief Processes spawn components and creates new entities when needed.
 * @details Checks each spawner's timer, creates new entities when time is up,
 * and resets timers. Spawned entities get random velocity.
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 * @param spriteManager Reference to the sprite manager
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
template <typename... Components>
void SpawnSystem(
    EntityManager<Components...>& entityManager, SpriteManager& spriteManager, float deltaTime)
{
    // Random number generator for spawned entity velocities
    // see: https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
    float maxStarVelocity = 50.0f;

    // Iterate through all entities
    for (const auto& entity : entityManager) {
        if (!entity.id || !entityManager.template HasComponent<SpawnComponent>(entity.id))
            continue;

        auto& spawn = entityManager.template GetComponent<SpawnComponent>(entity.id);

        // Update spawn timer
        spawn.timeToNextSpawn -= deltaTime;

        // Check if it's time to spawn
        if (spawn.timeToNextSpawn <= 0.0f) {
            // Check if we've reached max spawns
            if (spawn.maxSpawns != -1 && spawn.spawnCount >= spawn.maxSpawns)
                continue;

            // Get spawner location (spawners must have LocationComponent)
            assert(entityManager.template HasComponent<LocationComponent>(entity.id)
                && "Spawner entity must have LocationComponent!");

            const auto& spawnerLoc
                = entityManager.template GetComponent<LocationComponent>(entity.id);

            // Create new entity
            EntityID newEntity = entityManager.CreateEntity(spawn.entityType);

            // Add location component (at spawner position)
            entityManager.template AddComponent<LocationComponent>(
                newEntity, LocationComponent(spawnerLoc.position));

            // Add movement component with random velocity
            Point2D randomVelocity(velDist(gen), velDist(gen));
            entityManager.template AddComponent<MovementComponent>(
                newEntity, MovementComponent(randomVelocity, maxStarVelocity));

            // Get actual sprite dimensions from the registered sprite
            sf::Sprite& sprite = spriteManager.GetSprite(spawn.spriteID);
            sf::IntRect textureRect = sprite.getTextureRect();

            // Add sprite component
            // Note: Entity location is bottom-left, sprite rect is offset from that point
            // to get top-left corner for rendering
            Rect spriteBounds(0, -textureRect.size.y, textureRect.size.x, textureRect.size.y);
            entityManager.template AddComponent<SpriteComponent>(newEntity,
                SpriteComponent(
                    spawn.spriteID, spriteBounds, true, true) // true = world space, true = isAlive
            );

            // Add collision component matching sprite size
            // Collision box also offset from bottom-left location
            Rect collisionBox(0, -textureRect.size.y, textureRect.size.x, textureRect.size.y);
            entityManager.template AddComponent<CollisionComponent>(
                newEntity, CollisionComponent(collisionBox, false) // false = dynamic
            );

            // Reset spawn timer and increment count
            spawn.timeToNextSpawn = spawn.spawnInterval;
            spawn.spawnCount++;
        }
    }
}

} // namespace ECSEngine