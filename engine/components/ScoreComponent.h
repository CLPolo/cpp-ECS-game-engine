/**
 * @file ScoreComponent.h
 * @brief Score tracking and display component.
 */

#pragma once

#include "../managers/SpriteManager.h"
#include <cstddef>
#include <vector>

namespace ECSEngine {

using EntityID = size_t;

/**
 * @struct ScoreComponent
 * @brief Stores the current score, entities used to display it, and digit sprites.
 * @details Contains the score value, entity IDs for each digit display, and
 * sprite IDs corresponding to numbers 0-9.
 */
struct ScoreComponent {
    int score; // Current score value
    std::vector<EntityID> displayEntities; // Entities used to display score digits
    std::array<SpriteID, 10> digitSprites; // Sprite IDs for digits 0-9

    ScoreComponent()
        : score(0)
        , displayEntities()
        , digitSprites {}
    {
    }

    ScoreComponent(int initialScore)
        : score(initialScore)
        , displayEntities()
        , digitSprites {}
    {
    }
};

}
