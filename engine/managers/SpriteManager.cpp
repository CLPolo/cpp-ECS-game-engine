/**
 * @file SpriteManager.cpp
 * @brief Implementation of sprite and texture management.
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 */

#include "SpriteManager.h"
#include <iostream>

namespace ECSEngine {

[[nodiscard]] SpriteID SpriteManager::RegisterTexture(
    const std::string& texturePath, const Rect& sourceRect)
{
    // Load a texture from a file
    if (mTextures.find(texturePath) == mTextures.end()) { // !exist.
        sf::Texture texture(texturePath);
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error: Could not load texture: " << texturePath << std::endl;
            assert(false && "Failed to load texture!");
        }

        mTextures.emplace(texturePath, std::move(texture));
    }

    // Creating the Sprite
    sf::Sprite sprite(mTextures[texturePath]);

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(sourceRect.topLeft.x, sourceRect.topLeft.y),
        sf::Vector2i(sourceRect.width, sourceRect.height)));

    mSprites.emplace_back(std::move(sprite));

    return mSprites.size() - 1;
}

sf::Sprite& SpriteManager::GetSprite(SpriteID id)
{
    assert(id < mSprites.size());
    return mSprites[id];
}

} // namespace ECSEngine
