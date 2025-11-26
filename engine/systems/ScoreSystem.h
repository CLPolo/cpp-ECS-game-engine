/**
 * @file ScoreSystem.h
 * @brief Updates entities that display the score.
 */

#pragma once

#include "../components/ScoreComponent.h"
#include "../components/SpriteComponent.h"
#include "../managers/EntityManager.h"
#include <algorithm>
#include <cassert>
#include <vector>

namespace ECSEngine {

/**
 * @brief Updates entities that display the score.
 * @details Finds the entity with ScoreComponent and updates all display entities
 * to show the correct digit sprites based on the current score. The score is
 * displayed with leading zeros if it has fewer digits than display entities.
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 */
template <typename... Components> void ScoreSystem(EntityManager<Components...>& entityManager)
{
    // Find the entity with the score component
    for (const auto& entity : entityManager) {
        if (!entity.id || !entityManager.template HasComponent<ScoreComponent>(entity.id))
            continue;

        auto& scoreComp = entityManager.template GetComponent<ScoreComponent>(entity.id);

        // If no display entities, nothing to update
        if (scoreComp.displayEntities.empty())
            continue;

        size_t numDigits = scoreComp.displayEntities.size();
        std::vector<int> digits;
        digits.reserve(numDigits);

        // Extract digits from score (right to left)
        int value = std::min(scoreComp.score, 999);
        for (size_t i = 0; i < numDigits; ++i) {
            digits.push_back(value % 10);
            value /= 10;
        }

        // Reverse to get most significant digit first
        std::reverse(digits.begin(), digits.end());

        // Update each display entity with the corresponding digit sprite
        for (size_t i = 0; i < numDigits; ++i) {
            EntityID displayEntity = scoreComp.displayEntities[i];

            // Verify the display entity exists and has a sprite component
            if (!entityManager.ValidEntity(displayEntity))
                continue;

            if (!entityManager.template HasComponent<SpriteComponent>(displayEntity)) {
                assert(false && "Score display entity must have SpriteComponent!");
                continue; // allows program to ocntinue if release build
            }

            auto& spriteComp = entityManager.template GetComponent<SpriteComponent>(displayEntity);

            // Update the sprite to show the correct digit
            int digit = digits[i];
            assert(digit >= 0 && digit <= 9 && "Digit must be in range 0-9");
            spriteComp.spriteID = scoreComp.digitSprites[digit];
        }

        // Only process the first score component found
        break;
    }
}

} // namespace ECSEngine
