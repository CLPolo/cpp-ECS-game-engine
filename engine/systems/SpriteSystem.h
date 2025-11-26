/**
 * @file SpriteSystem.h
 * @brief Renders all sprites to the window.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <cassert>

#include "../components/CollisionComponent.h"
#include "../components/InputComponent.h"
#include "../components/LocationComponent.h"
#include "../components/SpriteComponent.h"
#include "../managers/EntityManager.h"
#include "../managers/SpriteManager.h"
#include "../managers/WindowManager.h"

#include <SFML/Graphics.hpp>

namespace ECSEngine {

/**
 * @brief Draws all entities with sprite components, then displays the window.
 * @details Iterates through all entities with SpriteComponent, converts world coordinates
 * to window coordinates for world-space sprites, and draws them. Screen-space sprites
 * are drawn at their absolute positions.
 *
 * Note: Entity locations represent the BOTTOM-LEFT corner of the entity. The spriteRect.topLeft
 * field provides an offset from this bottom-left position to determine where to render.
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 * @param spriteManager Reference to the sprite manager
 * @param windowManager Reference to the window manager
 */
template <typename... Components>
void SpriteSystem(EntityManager<Components...>& entityManager, SpriteManager& spriteManager,
    WindowManager& windowManager)
{
    sf::RenderWindow* window = windowManager.GetWindow();

    // Clear the window
    window->clear(sf::Color::Black);

    // Iterate through all entities
    for (const auto& entity : entityManager) {
        if (!entity.id || !entityManager.template HasComponent<SpriteComponent>(entity.id))
            continue;

        const auto& spriteComp = entityManager.template GetComponent<SpriteComponent>(entity.id);

        // If the 'sprite' is not alive, we won't draw it (i.e. stars)
        if (!spriteComp.isAlive) {
            continue;
        }

        // Get the sprite from the manager
        sf::Sprite& sprite = spriteManager.GetSprite(spriteComp.spriteID);

        // Determine position based on world space vs screen space
        Point2D position;

        if (spriteComp.worldSpace) {
            // World space sprites must have LocationComponent
            assert(entityManager.template HasComponent<LocationComponent>(entity.id)
                && "World-space sprite must have LocationComponent!");

            const auto& location
                = entityManager.template GetComponent<LocationComponent>(entity.id);

            // Calculate world position: location (bottom-left) + sprite offset
            Point2D worldPos = location.position + spriteComp.spriteRect.topLeft;

            // Convert world position to window position
            position = windowManager.WorldToWindow(worldPos);
        } else {
            // Screen space: use sprite rect position directly
            if (entityManager.template HasComponent<LocationComponent>(entity.id)) {
                const auto& location
                    = entityManager.template GetComponent<LocationComponent>(entity.id);

                position = location.position; // already in screen space

            } else {
                position = Point2D(0.0f, 0.0f); // Just in case
            }
        }

        // Set sprite position
        sprite.setPosition(sf::Vector2f(position.x, position.y));

        // Draw the sprite
        window->draw(sprite);
    }

    // Let'er rip, bud~!
    window->display();
}

} // namespace ECSEngine
