/**
 * @file SoundManager.h
 * @brief Sound effect management for the game engine.
 */
#pragma once

#include <cassert>
#include <iostream>
#include <unordered_map>

#include <SFML/Audio.hpp>

namespace ECSEngine {

/**
 * @class SoundManager
 * @brief Manages sound effects and music playback.
 * @details Handles sound loading, caching, and playback. Sounds are registered by
 * name and can be played multiple times. SoundBuffers are cached to avoid reloading.
 *
 * RESOURCE LIFETIME:
 * - Sounds and SoundBuffers are stored internally and remain valid for the lifetime
 *   of the SoundManager.
 *
 * - Sound names must be unique. Registering a sound with an existing name will
 *   reuse the cached SoundBuffer but create a new Sound instance.
 *
 * - Sounds can be played concurrently by calling PlaySound() multiple times.
 */
class SoundManager {
public:
    SoundManager() = default;
    ~SoundManager() = default;

    /**
     * @brief Registers a sound effect with a given name.
     * @param soundPath Path to the sound file to load
     * @param soundName Name to use when playing this sound
     */
    void RegisterSound(const std::string& soundPath, const std::string& soundName);

    /**
     * @brief Plays a sound effect by name.
     * @param soundName Name of the sound (must be registered first)
     */
    void PlaySound(const std::string& soundName);

private:
    std::unordered_map<std::string, sf::SoundBuffer> mSoundBuffers;
    std::unordered_map<std::string, sf::Sound> mSounds;
};

} // namespace ECSEngine