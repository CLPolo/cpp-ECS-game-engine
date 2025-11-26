#pragma once

#include <vector>

#include "core/MathUtil.h"
#include "managers/EntityManager.h"
#include "managers/SoundManager.h"
#include "managers/SpriteManager.h"
#include "managers/WindowManager.h"

#include "components/CameraComponent.h"
#include "components/CameraFollowerComponent.h"
#include "components/CameraShakeComponent.h"
#include "components/CollisionComponent.h"
#include "components/InputComponent.h"
#include "components/LocationComponent.h"
#include "components/ScoreComponent.h"
#include "components/SpawnComponent.h"
#include "components/SpriteComponent.h"
#include "components/TimeComponent.h"

#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CollisionSystemUpdate.h"
#include "systems/GravitySystem.h"
#include "systems/InputSystem.h"
#include "systems/MovementSystem.h"
#include "systems/ProcessEvents.h"
#include "systems/ScoreSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/SpriteSystem.h"
#include "systems/TimeSystem.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace ECSEngine {

template <typename... Components> class ECSEngine {
public:
    ECSEngine(unsigned int width, unsigned int height, const std::string& name);

    void Run();

    SoundManager& GetSoundManager() { return mSoundManager; }

    SpriteManager& GetSpriteManager() { return mSpriteManager; }

    WindowManager& GetWindowManager() { return mWindowManager; }

    EntityManager<Components...>& GetEntityManager() { return mEntityManager; }

private:
    EntityManager<Components...> mEntityManager;
    SpriteManager mSpriteManager;
    SoundManager mSoundManager;
    WindowManager mWindowManager;
};

template <typename... Components>
ECSEngine<Components...>::ECSEngine(
    unsigned int width, unsigned int height, const std::string& name)
    : mWindowManager(width, height, name)
{
}

template <typename... Components> void ECSEngine<Components...>::Run()
{
    // Delta time tracking
    sf::Clock clock;
    float deltaTime = 0.0f;

    // Main game loop - while window is still open
    while (mWindowManager.GetWindow()->isOpen()) {
        // Calculate delta time
        deltaTime = clock.restart().asSeconds();

        // Run all systems in order
        ProcessEvents(mEntityManager, mWindowManager);
        CollisionSystemUpdate(mEntityManager);
        InputSystem(mEntityManager, mSoundManager, deltaTime);
        GravitySystem(mEntityManager, deltaTime);
        MovementSystem(mEntityManager, deltaTime);
        CollisionSystem(mEntityManager, mSoundManager);
        ScoreSystem(mEntityManager);
        TimeSystem(mEntityManager, deltaTime); // Update timers before systems that check them
        CameraSystem(mEntityManager, mWindowManager, deltaTime);
        SpriteSystem(mEntityManager, mSpriteManager, mWindowManager);
        SpawnSystem(mEntityManager, mSpriteManager, deltaTime);
    }
}

} // namespace ECSEngine
