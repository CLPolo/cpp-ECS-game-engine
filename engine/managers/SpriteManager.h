/**
 * @file SpriteManager.h
 * @brief Sprite and texture management for the game engine.
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include <cassert>
#include <unordered_map>
#include <vector>

#include "../core/MathUtil.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

using SpriteID = size_t;

/**
 * @class SpriteManager
 * @brief Manages sprites and their associated textures.
 * @details Handles texture loading, caching, and sprite creation. Textures are
 * stored in an unordered_map to ensure stable memory addresses (required by SFML).
 * Sprites are stored in a vector and accessed by SpriteID (the vector index).
 *
 * RESOURCE LIFETIME:
 * - sf::Sprite references (from GetSprite()) are valid until the SpriteManager
 *   is destroyed. Do NOT store these references long-term.
 *
 * - Textures are cached by file path. Registering the same texture path multiple
 *   times reuses the existing texture.
 *
 * - SpriteIDs remain stable and valid for the lifetime of the SpriteManager.
 */
class SpriteManager {
public:
    SpriteManager() = default;
    ~SpriteManager() = default;

    /**
     * @brief Registers a texture and creates a sprite from it.
     * @param texturePath Path to the texture file to load
     * @param sourceRect The portion of the texture to use for this sprite
     * @return SpriteID that can be used with GetSprite()
     */
    [[nodiscard]] SpriteID RegisterTexture(const std::string& texturePath, const Rect& sourceRect);

    /**
     * @brief Gets a reference to a sprite by its ID.
     * @param id The SpriteID returned from RegisterTexture()
     * @return Reference to sf::Sprite (valid for lifetime of SpriteManager)
     */
    sf::Sprite& GetSprite(SpriteID id);

private:
    std::unordered_map<std::string, sf::Texture> mTextures;
    std::vector<sf::Sprite> mSprites;
};

} // namespace ECSEngine