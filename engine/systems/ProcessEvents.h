/**
 * @file ProcessEvents.h
 * @brief Reads keyboard event and updates any that are down.
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include <cassert>
#include "../managers/EntityManager.h"
#include "../managers/WindowManager.h"
#include "../components/InputComponent.h"
#include <SFML/Window.hpp>

namespace ECSEngine {

/**
 * @brief This system reads SFML keyboard events and updates the map of
 * what keys are currently down (T/F) on any entities with the input component.
 *
 * @tparam Components The components for an entity.
 * @param window Reference to the window.
 * @param entityManager Reference to the entity manager.
 */
template <typename... Components>
void ProcessEvents(EntityManager<Components...>& entityManager, WindowManager& windowManager)
{
    sf::RenderWindow* window = windowManager.GetWindow();

    while (auto event = window->pollEvent()) {

        // Check for window close event
        if (event->is<sf::Event::Closed>()) {
            window->close();
        }

        // Process keyboard events for all entities with InputComponent
        for (const auto& entity : entityManager) {

            if (!entity.id || !entityManager.template HasComponent<InputComponent>(entity.id)) {
                continue;
            }

            auto& input = entityManager.template GetComponent<InputComponent>(entity.id);

            // Handle key pressed event
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                input.keydown.set(static_cast<size_t>(keyPressed->scancode), true);
            }
            // Handle key released event
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                input.keydown.set(static_cast<size_t>(keyReleased->scancode), false);
            }
        }
    }
}

} // namespace ECSEngine
