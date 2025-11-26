/**
 * @file ScoreLoader.h
 * @brief Handles loading the score.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "components/SpriteComponent.h"
#include "managers/EntityManager.h"
#include "managers/SpriteManager.h"

namespace ECSEngine {

constexpr int NUM_TO_DISPLAY = 3;

/**
 * @struct ScoreDisplay
 * @brief Holds data for displayEntities and digitSprites.
 */
struct ScoreDisplay {
    std::vector<EntityID> displayEntities;
    std::array<SpriteID, 10> digitSprites;
};

/**
 * @brief Create a Score object.
 *
 * @tparam Components The component types managed by the EntityManager.
 * @param entityManager Reference to EntityManager.
 * @param spriteManager Reference to SpriteManager.
 * @param path Path to the sprite sheet: "spritesheet-tiles-default.png"
 * @param numDigits Total digits to process: 0...9
 * @return ScoreDisplay Struct with score data.
 */
template <typename... Components>
ScoreDisplay CreateScore(EntityManager<Components...>& entityManager, SpriteManager& spriteManager,
    const std::string& path, int numDigits)
{
    ScoreDisplay data;
    data.displayEntities.reserve(NUM_TO_DISPLAY);

    float spacing = 40.0f; // space between digits

    // Fills digitSprites
    for (int i = 0; i < numDigits; ++i) {

        EntityID digit = entityManager.CreateEntity("Num" + std::to_string(i));

        Rect digitRect(832, 832 - (64 * i), 64, 64); // Row 13, Col X, and 64 x 64
        SpriteID spriteId = spriteManager.RegisterTexture(path, digitRect);

        SpriteComponent digitSpriteComp(spriteId, digitRect, false, true);

        // To Restrict Display to NUM_TO_DISPLAY digits
        if (i >= NUM_TO_DISPLAY) {
            digitSpriteComp.isAlive = false;
        }

        entityManager.template AddComponent<SpriteComponent>(digit, digitSpriteComp);

        LocationComponent location(0.0f + (i * spacing), 0.0f);
        entityManager.template AddComponent<LocationComponent>(digit, location);

        data.digitSprites[i] = spriteId;

        // Initializing displayEntries to NUM_TO_DISPLAY digits
        if (i < NUM_TO_DISPLAY) {
            data.displayEntities.push_back(digit);
        }
    }

    return data;
}

} // namespace ECSEngine
