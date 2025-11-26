/**
 * @file SpawnComponent.h
 * @brief Contains data needed for spawning entities.
 */

#pragma once

#include <unordered_map>
#include "../core/MathUtil.h"
#include "../managers/SpriteManager.h"

namespace ECSEngine {

using EntityID = size_t;

/**
 * @struct SpawnComponent
 * @brief Handles spawning data for entities.
 *
 * @details Sprite rectangle is relative to the entity location in the world and is only
 * defined if an entity also has a LocationComponent.
 */
struct SpawnComponent {

    EntityID entityID;           // Entity this component is attached to
    std::string entityType;      // Description of what should be spawned
    SpriteID spriteID;           // Sprite/texture of object to spawn

    float timeToNextSpawn;
    float spawnInterval;         // Time between spawns
    int spawnCount;
    int maxSpawns;               // Maximum spawns (-1 for unlimited)


    SpawnComponent()
        : entityID(0)
        , entityType("")
        , spriteID(0)
        , timeToNextSpawn(0.0f)
        , spawnInterval(1.0f)
        , spawnCount(0)
        , maxSpawns(-1)
    {
    }
    SpawnComponent(size_t entity, const std::string& type, SpriteID spriteID, float interval)
        : entityID(entity)
        , entityType(type)
        , spriteID(spriteID)
        , timeToNextSpawn(0.0f)
        , spawnInterval(interval)
        , spawnCount(0)
    {
    }
};

} // namespace ECSEngine
