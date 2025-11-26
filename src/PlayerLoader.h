/**
 * @file PlayerLoader.h
 * @brief Handles player loading.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "ScoreLoader.h"
#include "components/AccelerationComponent.h"
#include "components/CollisionComponent.h"
#include "components/InputComponent.h"
#include "components/LocationComponent.h"
#include "components/MovementComponent.h"
#include "components/ScoreComponent.h"
#include "components/SpriteComponent.h"
#include "components/TimeComponent.h"
#include "core/MathUtil.h"
#include "managers/EntityManager.h"
#include "managers/SpriteManager.h"

namespace ECSEngine {

constexpr float PLAYER_ACCELERATION = 1200.0f; // Horizontal acceleration
constexpr float PLAYER_MAX_SPEED = 300.0f;

/**
 * @brief Create a Player object.
 *
 * @tparam Components The component types managed by the EntityManager.
 * @param skinPath Path to the sprite sheet: "spritesheet-characters-default.png"
 * @param tilesPath Path to the sprite sheet: "spritesheet-tiles-default.png"
 * @param entityManager Reference to EntityManager.
 * @param spriteManager Reference to SpriteManager.
 * @return EntityID Entity id for the player.
 */
template <typename... Components>
EntityID CreatePlayer(const std::string skinPath, const std::string tilesPath,
    EntityManager<Components...>& entityManager, SpriteManager& spriteManager)
{
    EntityID player = entityManager.CreateEntity("player");

    LocationComponent location;
    entityManager.AddComponent(player, location);

    MovementComponent movement(0.0f, 0.0f, PLAYER_MAX_SPEED);
    entityManager.AddComponent(player, movement);

    AccelerationComponent acceleration(PLAYER_ACCELERATION, 0.0f);
    entityManager.AddComponent(player, acceleration);

    InputComponent input;
    entityManager.AddComponent(player, input);

    /* A character is 128 x 128 in our sprite sheet.
     * I want the green character, which is row 1 and col 5.
     * Bon Appetit!
     */

    const float height = 128.0f; // Character is 128 x 128
    const float width = 128.0f;

    int row = 0;
    int col = 4;

    Rect chosenSkin(Point2D(col * width, row * height), width, height);
    SpriteID spriteId = spriteManager.RegisterTexture(skinPath, chosenSkin);

    Rect spriteRect(Point2D(0.0f, -height), width, height);
    SpriteComponent sprite(spriteId, spriteRect, true);
    entityManager.AddComponent(player, sprite);

    // Converts to bottom-left offsets (Bounding box measured from sprite's top-left)
    Point2D bbTopLeft(width * 0.25f, -height + height * 0.5f);
    Rect boundingBox(bbTopLeft, width * 0.5f, height * 0.5f);
    CollisionComponent collision(boundingBox, false);
    entityManager.AddComponent(player, collision);

    // Attaches score to the player
    auto scoreDisplay = CreateScore(entityManager, spriteManager, tilesPath, 10);
    ScoreComponent score;
    score.displayEntities = scoreDisplay.displayEntities;
    score.digitSprites = scoreDisplay.digitSprites;
    entityManager.AddComponent(player, score);

    return player;
}

} // namespace ECSEngine
