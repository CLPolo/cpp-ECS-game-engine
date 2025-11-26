/**
 * @file TimeComponent.h
 * @brief Countdown timer tracking.
 * @version 0.1
 * @date 2025-11-08
 *
 * @copyright Copyright (c) 2025
 */

#ifndef TIMECOMPONENT_H
#define TIMECOMPONENT_H

namespace ECSEngine {

/**
 * @struct TimeComponent
 * @brief Time-based countdown timer for entities.
 * @details Uses deltaTime for accurate, frame-rate independent timing.
 * Managed by TimeSystem which decrements timeRemaining each frame.
 */
struct TimeComponent {
    float totalDuration;     // Total duration in seconds
    float timeRemaining;     // Time remaining in seconds
    bool isRunning;          // Whether timer is actively counting down
    bool restart;            // If true, timer resets to totalDuration when it reaches 0

    TimeComponent()
        : totalDuration(0.0f)
        , timeRemaining(0.0f)
        , isRunning(false)
        , restart(false)
    {
    }

    TimeComponent(float duration, bool autoRestart = false)
        : totalDuration(duration)
        , timeRemaining(duration)
        , isRunning(duration > 0.0f)
        , restart(autoRestart)
    {
    }
};

} // namespace ECSEngine

#endif // TIMECOMPONENT_H