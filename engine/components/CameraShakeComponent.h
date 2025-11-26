/**
 * @file CameraShakeComponent.h
 * @brief Handles shaking the camera.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include "../core/MathUtil.h"
#include <cstddef>

namespace ECSEngine {

using EntityID = size_t;

/**
 * @struct CameraShake
 * @brief Stores shake parameters for screen shake effects.
 * @details Shake uses sine/cosine oscillation. Higher frequency = faster vibration.
 *
 * IMPORTANT: This component stores shake *parameters* only. Timing is handled by
 * attaching a TimeComponent to the same entity. When the TimeComponent is running,
 * CameraSystem applies the shake. When TimeComponent expires, shake stops.
 *
 * Usage:
 *   1. Add CameraShake component with magnitude/frequency parameters
 *   2. Add TimeComponent with desired shake duration
 *   3. Other systems (e.g., CollisionSystem) set isShaking = true with direction
 *   4. CameraSystem configures shake parameters and starts timer when isShaking
 *   5. TimeSystem manages countdown, CameraSystem applies shake while timer runs
 */
struct CameraShake {
    bool isShaking;   // Flag set by other systems to trigger shake
    bool horizontal;  // True for horizontal shake, false for vertical
    float magnitudeX; // Shake intensity in X direction
    float magnitudeY; // Shake intensity in Y direction
    float frequencyX; // Oscillation frequency for X (higher = faster shake)
    float frequencyY; // Oscillation frequency for Y (higher = faster shake)
    float elapsedTime; // Accumulated time for oscillation calculation

    CameraShake()
        : isShaking(false)
        , horizontal(false)
        , magnitudeX(0.0f)
        , magnitudeY(0.0f)
        , frequencyX(1.5f)
        , frequencyY(2.1f)
        , elapsedTime(0.0f)
    {
    }

    CameraShake(float magX, float magY, float freqX = 1.5f, float freqY = 2.1f)
        : isShaking(false)
        , horizontal(false)
        , magnitudeX(magX)
        , magnitudeY(magY)
        , frequencyX(freqX)
        , frequencyY(freqY)
        , elapsedTime(0.0f)
    {
    }
};

} // namespace ECSEngine