/**
 * @file main.cpp
 * @brief Handles everything needed for the game itself.
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 */

#include <iostream>
#include <unordered_set>

#include "MapLoader.h"
#include "PlayerLoader.h"
#include "ScoreLoader.h"
#include "components/AccelerationComponent.h"
#include "components/CollisionComponent.h"
#include "components/InputComponent.h"
#include "components/LocationComponent.h"
#include "components/MovementComponent.h"
#include "components/ScoreComponent.h"
#include "components/SpawnComponent.h"
#include "components/SpriteComponent.h"
#include "core/ECSEngine.h"
#include "core/MathUtil.h"
#include "managers/EntityManager.h"
#include "managers/SoundManager.h"
#include "managers/SpriteManager.h"

std::string gResourcePath = "../../assets/";

int main(int argc, char* argv[])
{
    bool debugMode = false;
    if (argc >= 3 && argv[1] == std::string("-path")) {
        gResourcePath = argv[2];
    }
    std::cout << "Usage: " << argv[0] << " [-path resource_path]\n";
    std::cout << "Using resource path: " << gResourcePath << "\n";

    // Generic entity manager testing
    if (debugMode) {
        ECSEngine::EntityManager<int, float, bool> e;
        //    e1.AddComponent(42);
        ECSEngine::EntityID e1 = e.CreateEntity("test");
        e.AddComponent(e1, 42);
        e.AddComponent(e1, 3.14f);
        std::cout << e.HasComponent<bool>(e1) << "\n";
        std::cout << e.HasComponent<float>(e1) << "\n";
        std::cout << e.GetComponent<float>(e1) << "\n";
        std::cout << e.HasComponent<int>(e1) << "\n";
        e.GetComponent<int>(e1) = 31415;
        std::cout << e.GetComponent<int>(e1) << "\n";
        return 0;
    }

    // ECS Engine Part 1 startup code
    ECSEngine::ECSEngine<ECSEngine::LocationComponent, ECSEngine::MovementComponent,
        ECSEngine::AccelerationComponent, ECSEngine::CollisionComponent, ECSEngine::SpriteComponent,
        ECSEngine::SpawnComponent, ECSEngine::CameraComponent, ECSEngine::CameraFollower,
        ECSEngine::InputComponent, ECSEngine::CameraShake, ECSEngine::ScoreComponent,
        ECSEngine::TimeComponent>
        engine(1024, 768, "Test Engine");

    auto& spriteManager = engine.GetSpriteManager();
    auto& entityManager = engine.GetEntityManager();
    auto& soundManager = engine.GetSoundManager();

    // The Necessary Paths
    const std::string skyMapPath = gResourcePath + "sky.map";
    const std::string worldMapPath = gResourcePath + "world.map";
    const std::string jumpPath = gResourcePath + "sfx_jump.ogg";
    const std::string gemPath = gResourcePath + "sfx_gem.ogg";
    const std::string tilesTexturePath = gResourcePath + "spritesheet-tiles-default.png";
    const std::string playerSkinPath = gResourcePath + "spritesheet-characters-default.png";

    // Registering Sounds
    soundManager.RegisterSound(jumpPath, "jump");
    soundManager.RegisterSound(gemPath, "sparkle");

    // Loads Background and Gameplay Maps (before starting the main loop)
    LoadMapLayer(skyMapPath, gResourcePath, entityManager, spriteManager);
    const std::unordered_set<char> nonCollidableSymbols { 'S' };
    const auto worldLayer = LoadMapLayer(
        worldMapPath, gResourcePath, entityManager, spriteManager, nonCollidableSymbols);

    std::cout << "Loaded " << worldLayer.GetEntities('S').size() << " spawners from world map.\n";

    // Sets up Spawners with SpawnComponent
    // Register star sprite
    ECSEngine::Rect starSpriteRect(640, 320, 64, 64);
    ECSEngine::SpriteID starSpriteID
        = spriteManager.RegisterTexture(tilesTexturePath, starSpriteRect);

    // Configure each spawner entity
    for (ECSEngine::EntityID spawnerID : worldLayer.GetEntities('S')) {

        // Add SpawnComponent to spawner
        ECSEngine::SpawnComponent spawnComp(
            spawnerID, "star", starSpriteID, 3.0f); // Spawn interval: 3 seconds
        spawnComp.maxSpawns = 10; // set to -1 for unlimited
        entityManager.AddComponent(spawnerID, spawnComp);
    }

    std::cout << "Configured " << worldLayer.GetEntities('S').size() << " spawners.\n";

    // Player Time!
    ECSEngine::EntityID player
        = CreatePlayer(playerSkinPath, tilesTexturePath, entityManager, spriteManager);

    std::cout << "Created Player!\n";

    // Creates Camera Entity
    ECSEngine::EntityID cameraEntity = entityManager.CreateEntity("main_camera");
    ECSEngine::CameraComponent camera;
    camera.position = ECSEngine::Point2D(0.0f, 384.0f);
    camera.worldUnitsPerPixel = 1.0f; // 1 world unit = 1 pixel
    entityManager.AddComponent(cameraEntity, camera);

    // Camera should follow the player on X, keep Y fixed per spec
    ECSEngine::CameraFollower follower(player, true, false);
    entityManager.AddComponent(cameraEntity, follower);

    // Camera needs a time component for shaking
    ECSEngine::TimeComponent shakeTimer(0.2f);
    entityManager.AddComponent(cameraEntity, shakeTimer);

    // Camera should shake when player hits a solid object
    ECSEngine::CameraShake shake(5.0f, 2.0f, 5.0f, 2.1f); // magnitudeX/Y frequencyX/Y
    entityManager.AddComponent(cameraEntity, shake);

    std::cout << "Established Camera!\n";

    // Creates Lonely Star
    ECSEngine::EntityID lonelyStar = entityManager.CreateEntity("star");
    ECSEngine::LocationComponent starLocation(-64.0f, 256.0f);
    ECSEngine::Rect lonelyStarSpriteRect(
        { starLocation.position.x, starLocation.position.y - 64.0f }, 64.0f, 64.0f);
    entityManager.AddComponent(lonelyStar, starLocation);

    ECSEngine::SpriteComponent starSprite(starSpriteID, lonelyStarSpriteRect, true, true);
    entityManager.AddComponent(lonelyStar, starSprite);

    ECSEngine::CollisionComponent starCollision(lonelyStarSpriteRect, false);
    entityManager.AddComponent(lonelyStar, starCollision);

    std::cout << "Created the Lonely Star\n";

    // Runs the game
    engine.Run();

    return 0;
}
