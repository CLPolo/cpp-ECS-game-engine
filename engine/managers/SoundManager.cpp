/**
 * @file SoundManager.cpp
 * @brief Implementation of sound effect management.
 */

#include "SoundManager.h"

namespace ECSEngine {

void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName)
{
    // Load A Soundbuffer From a File
    if (mSoundBuffers.find(soundName) == mSoundBuffers.end()) { // !exist.

        sf::SoundBuffer soundBuffer;
        if (!soundBuffer.loadFromFile(soundPath)) {
            std::cerr << "Error: Could not load sound." << std::endl;
        }

        mSoundBuffers.emplace(soundName, std::move(soundBuffer));
    }

    // Creating and Store the Sound
    sf::Sound sound(mSoundBuffers[soundName]);
    mSounds.emplace(soundName, std::move(sound));
}

void SoundManager::PlaySound(const std::string& soundName)
{
    auto theSound = mSounds.find(soundName);
    assert(theSound != mSounds.end() && "Sound not found!");
    theSound->second.play();
}

} // namespace ECSEngine