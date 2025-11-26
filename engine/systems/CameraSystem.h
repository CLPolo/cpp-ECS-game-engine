/**
 * @file CameraSystem.h
 * @brief Camera movement and following system.
 */

#pragma once

#include <cassert>
#include <cmath>

#include "../managers/EntityManager.h"
#include "../managers/WindowManager.h"
#include "../components/CameraComponent.h"
#include "../components/LocationComponent.h"
#include "../components/TimeComponent.h"

namespace ECSEngine
{

/**
 * @brief Updates camera position to follow entities with CameraFollower component.
 * @details Implements smooth camera following with deadzone behavior similar to lab 8.
 * Following is configurable per-axis via CameraFollower.followX and followY flags.
 *
 * Camera shake is applied if the camera entity has both CameraShake and TimeComponent.
 * The shake effect is active while TimeComponent.isRunning is true.
 *
 * Camera zones (applied per axis if following is enabled):
 * - Outer 10%: Direct follow (entity at edge)
 * - Middle 10-30%: Smooth tracking/tween
 * - Center 40%: No movement (deadzone)
 *
 * @tparam Components The component types in the EntityManager
 * @param entityManager Reference to the entity manager
 * @param windowManager Reference to the window manager
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
template <typename... Components>
void CameraSystem(EntityManager<Components...>& entityManager,
                  WindowManager& windowManager,
                  float deltaTime)
{
    // Find the camera entity
    EntityID cameraEntity = 0;
    bool foundCamera = false;

    for (const auto& entity : entityManager)
    {
        if (entity.id && entityManager.template HasComponent<CameraComponent>(entity.id))
        {
            cameraEntity = entity.id;
            foundCamera = true;
            break;
        }
    }

    // If no camera exists, nothing to do
    if (!foundCamera)
        return;

    auto& camera = entityManager.template GetComponent<CameraComponent>(cameraEntity);

    // Handle camera shake if present (requires TimeComponent for timing)
    Point2D shakeOffset(0.0f, 0.0f);
    if (entityManager.template HasComponent<CameraShake>(cameraEntity))
    {
        // Enforce TimeComponent requirement
        assert(entityManager.template HasComponent<TimeComponent>(cameraEntity)
               && "Entity with CameraShake must have TimeComponent for timing!");

        auto& shake = entityManager.template GetComponent<CameraShake>(cameraEntity);
        auto& timer = entityManager.template GetComponent<TimeComponent>(cameraEntity);

        // Check if shake was triggered by another system
        if (shake.isShaking)
        {
            // Restart the shake timer
            timer.isRunning = true;
            timer.timeRemaining = timer.totalDuration;
            shake.elapsedTime = 0.0f;

            // Clear the trigger flag
            shake.isShaking = false;
        }

        // Apply shake while timer is running
        if (timer.isRunning && timer.timeRemaining > 0.0f)
        {
            // Update elapsed time for oscillation
            shake.elapsedTime += deltaTime;

            // Fade shake intensity based on remaining time
            float intensity = timer.timeRemaining / timer.totalDuration;

            // Apply shake based on direction
            if (shake.horizontal)
            {
                // Horizontal shake (wall hit)
                float randomX = std::sin(shake.elapsedTime * shake.frequencyX);
                shakeOffset.x = shake.magnitudeX * randomX * intensity;
            }
            else
            {
                // Vertical shake (landing)
                float randomY = std::cos(shake.elapsedTime * shake.frequencyY);
                shakeOffset.y = shake.magnitudeY * randomY * intensity;
            }
        }
        else
        {
            // Timer expired, reset elapsed time for next shake
            shake.elapsedTime = 0.0f;
        }
    }

    // Check if camera has a follower component
    if (!entityManager.template HasComponent<CameraFollower>(cameraEntity))
    {
        // No follower, just apply shake and update window
        Point2D finalPosition = camera.position + shakeOffset;
        windowManager.SetCamera(finalPosition);
        windowManager.SetWorldScale(camera.worldUnitsPerPixel);
        return;
    }

    auto& follower = entityManager.template GetComponent<CameraFollower>(cameraEntity);

    // Check if the entity being followed exists and has a location
    if (!entityManager.ValidEntity(follower.entityToTrack) ||
        !entityManager.template HasComponent<LocationComponent>(follower.entityToTrack))
    {
        // Entity doesn't exist or has no location, just apply shake
        Point2D finalPosition = camera.position + shakeOffset;
        windowManager.SetCamera(finalPosition);
        windowManager.SetWorldScale(camera.worldUnitsPerPixel);
        return;
    }

    const auto& targetLocation = entityManager.template GetComponent<LocationComponent>(follower.entityToTrack);

    // Get window dimensions to calculate zones
    sf::RenderWindow* window = windowManager.GetWindow();
    unsigned int windowWidth = window->getSize().x;
    unsigned int windowHeight = window->getSize().y;

    // Camera following parameters
    float outerEdgePercent = 0.10f;      // 10% outer edge - direct follow
    float trackingZonePercent = 0.30f;   // 10-30% zone - smooth tracking
    float tweenSpeed = 0.05f;            // Smooth tracking speed

    // Convert target world position to window coordinates
    Point2D targetWindowPos = windowManager.WorldToWindow(targetLocation.position);

    Point2D cameraAdjust(0.0f, 0.0f);

    // Handle X-axis following
    if (follower.followX)
    {
        float outerEdge = windowWidth * outerEdgePercent;
        float trackingZone = windowWidth * trackingZonePercent;
        float windowCenterX = windowWidth / 2.0f;
        float offsetX = targetWindowPos.x - windowCenterX;

        // Determine camera movement based on zone
        if (offsetX > windowCenterX - outerEdge)
        {
            // Right edge - follow directly
            cameraAdjust.x = offsetX - (windowCenterX - outerEdge);
        }
        else if (offsetX < -(windowCenterX - outerEdge))
        {
            // Left edge - follow directly
            cameraAdjust.x = offsetX + (windowCenterX - outerEdge);
        }
        else if (offsetX > windowCenterX - trackingZone)
        {
            // Right tracking zone - smooth follow
            cameraAdjust.x = (offsetX - (windowCenterX - trackingZone)) * tweenSpeed;
        }
        else if (offsetX < -(windowCenterX - trackingZone))
        {
            // Left tracking zone - smooth follow
            cameraAdjust.x = (offsetX + (windowCenterX - trackingZone)) * tweenSpeed;
        }
        // Otherwise in center 40% - no adjustment
    }

    // Handle Y-axis following
    if (follower.followY)
    {
        float outerEdge = windowHeight * outerEdgePercent;
        float trackingZone = windowHeight * trackingZonePercent;
        float windowCenterY = windowHeight / 2.0f;
        float offsetY = targetWindowPos.y - windowCenterY;

        // Determine camera movement based on zone
        if (offsetY > windowCenterY - outerEdge)
        {
            // Bottom edge - follow directly
            cameraAdjust.y = offsetY - (windowCenterY - outerEdge);
        }
        else if (offsetY < -(windowCenterY - outerEdge))
        {
            // Top edge - follow directly
            cameraAdjust.y = offsetY + (windowCenterY - outerEdge);
        }
        else if (offsetY > windowCenterY - trackingZone)
        {
            // Bottom tracking zone - smooth follow
            cameraAdjust.y = (offsetY - (windowCenterY - trackingZone)) * tweenSpeed;
        }
        else if (offsetY < -(windowCenterY - trackingZone))
        {
            // Top tracking zone - smooth follow
            cameraAdjust.y = (offsetY + (windowCenterY - trackingZone)) * tweenSpeed;
        }
        // Otherwise in center 40% - no adjustment
    }

    // Update camera position
    camera.position.x += cameraAdjust.x * camera.worldUnitsPerPixel;
    camera.position.y += cameraAdjust.y * camera.worldUnitsPerPixel;

    // Apply final position with shake
    Point2D finalPosition = camera.position + shakeOffset;
    windowManager.SetCamera(finalPosition);
    windowManager.SetWorldScale(camera.worldUnitsPerPixel);
}

} // namespace ECSEngine
