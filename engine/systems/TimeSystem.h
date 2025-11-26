/**
 * @file TimeSystem.h
 * @brief Handles countdown timers.
 * @version 0.1
 * @date 2025-11-08
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "../components/TimeComponent.h"
#include "../managers/EntityManager.h"

namespace ECSEngine {

/**
 * @brief Updates all TimeComponents attached to entities.
 *
 * @details Decrements the remaining time of each timer using deltaTime for frame-rate
 * independent timing. If a timer finishes, it either stops or restarts automatically
 * if `restart` is true.
 *
 * @tparam Components The components for an entity.
 * @param entityManager Reference to the Entity Manager.
 * @param deltaTime Time elapsed since last frame (in seconds).
 */
template <typename... Components>
void TimeSystem(EntityManager<Components...>& entityManager, float deltaTime)
{
    for (const auto& entity : entityManager) {

        if (!entity.id || !entityManager.template HasComponent<TimeComponent>(entity.id)) {
            continue;
        }

        auto& timer = entityManager.template GetComponent<TimeComponent>(entity.id);
        if (!timer.isRunning) {
            continue;
        }

        // Decrease timer by deltaTime
        timer.timeRemaining -= deltaTime;

        // When timer finishes
        if (timer.timeRemaining <= 0.0f) {

            if (timer.restart) {
                // Reset for next cycle
                timer.timeRemaining = timer.totalDuration;
            } else {
                // Stop the timer
                timer.isRunning = false;
                timer.timeRemaining = timer.totalDuration;
            }
        }
    }
}

} // namespace ECSEngine