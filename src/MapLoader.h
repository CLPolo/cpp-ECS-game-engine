/**
 * @file MapLoader.h
 * @brief Game-specific map loading for platformer tile maps.
 * @details Loads map files with dictionary-based sprite definitions and tile grids.
 * Supports two dictionary types:
 *   - Dictionary 1: Background sprites (no collision)
 *   - Dictionary 2: Gameplay sprites (with collision bounding boxes)
 */

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "managers/EntityManager.h"
#include "managers/SpriteManager.h"
#include "core/MathUtil.h"
#include "components/LocationComponent.h"
#include "components/CollisionComponent.h"
#include "components/SpriteComponent.h"

extern std::string gResourcePath;

/**
 * @struct TileDef
 * @brief Defines a single tile type from the map dictionary.
 */
struct TileDef {
    char symbol;                // Character used in map grid
    std::string texturePath;    // Path to sprite sheet
    ECSEngine::Rect spriteRect; // Source rectangle in sprite sheet
    bool hasCollision;          // Whether this tile has collision (dictionary 2)
    ECSEngine::Rect boundingBox;// Collision bounding box relative to tile (dictionary 2)
};

/**
 * @struct MapLayerData
 * @brief Contains information about entities created from a map layer.
 */
class MapLayerData {
public:
    /**
     * @brief Get all entity IDs that were created from a specific symbol.
     * @param symbol The character symbol from the map
     * @return Vector of EntityIDs that used this symbol
     */
    const std::vector<ECSEngine::EntityID>& GetEntities(char symbol) const {
        auto it = mSymbolEntities.find(symbol);
        if (it != mSymbolEntities.end()) {
            return it->second;
        }
        static const std::vector<ECSEngine::EntityID> empty;
        return empty;
    }

    /**
     * @brief Add an entity to the tracking for a specific symbol.
     * @param symbol The character symbol from the map
     * @param entityID The entity that was created
     */
    void AddEntity(char symbol, ECSEngine::EntityID entityID) {
        mSymbolEntities[symbol].push_back(entityID);
    }

private:
    std::unordered_map<char, std::vector<ECSEngine::EntityID>> mSymbolEntities;
};

/**
 * @brief Load a map layer from a file and create entities.
 * @details Parses a map file with the following format:
 *
 *   dictionary [1 or 2]
 *   [char] [path] [sprite rect] [bounding box (dict 2 only)]
 *   ...
 *   map origin [x y] tile [width height] size [grid_width grid_height]
 *   [grid of characters]
 *
 * Dictionary 1: Background tiles (no collision)
 *   Format: [char] [path] [x y width height]
 *
 * Dictionary 2: Gameplay tiles (with collision)
 *   Format: [char] [path] [sprite x y w h] [bbox x y w h]
 *
 * @tparam Components The component types managed by the EntityManager
 * @param mapFilePath Path to the .map file
 * @param resourcePath Base path for resolving texture paths
 * @param entityManager Reference to the entity manager for creating entities
 * @param spriteManager Reference to the sprite manager for registering textures
 * @param nonCollidableSymbols Set of symbols that should not have collision (e.g., 'S' for spawners)
 * @return MapLayerData containing information about created entities
 */
template <typename... Components>
MapLayerData LoadMapLayer(
    const std::string& mapFilePath,
    const std::string& resourcePath,
    ECSEngine::EntityManager<Components...>& entityManager,
    ECSEngine::SpriteManager& spriteManager,
    const std::unordered_set<char>& nonCollidableSymbols = {})
{
    MapLayerData layerData;
    std::ifstream file(mapFilePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open map file: " << mapFilePath << std::endl;
        return layerData;
    }

    // Parse dictionary type
    // see: https://www.geeksforgeeks.org/cpp/processing-strings-using-stdistringstream/
    std::string line;
    std::getline(file, line);
    std::istringstream dictLine(line);
    std::string dictWord;
    int dictType;

    // line = "dictionary n" >> dictWord = "dictionary", dictLine = "n" >> dictType = "n", line = EOL
    // note: ">>" operator returns a reference to the input stream, and the chain is evaluated left to right.
    //       Thus we have:
    //          (dictLine >> dictWord) >> dictType;
    //          (result) >> dictType
    //       where "result" is the dictLine istringstream pointer to the second whitespace-delimited string
    dictLine >> dictWord >> dictType;

    // Map file must abide by expected format
    if (dictWord != "dictionary" || (dictType != 1 && dictType != 2)) {
        std::cerr << "Error: Invalid dictionary declaration in " << mapFilePath << std::endl;
        return layerData;
    }

    // Parse tile definitions
    std::unordered_map<char, TileDef> tileDefs;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Check if we've reached the map section
        if (line.find("map origin") == 0) {
            break;
        }

        std::istringstream iss(line);
        TileDef tileDef;
        iss >> tileDef.symbol >> tileDef.texturePath;

        if (dictType == 1) {
            // Dictionary 1: [char] [path] [x y width height]
            float x, y, w, h;
            iss >> x >> y >> w >> h;
            tileDef.spriteRect = ECSEngine::Rect(x, y, w, h);
            tileDef.hasCollision = false;
        } else {
            // Dictionary 2: [char] [path] [sprite x y w h] [bbox x y w h]
            float sx, sy, sw, sh;  // sprite rect
            float bx, by, bw, bh;  // bounding box
            iss >> sx >> sy >> sw >> sh >> bx >> by >> bw >> bh;
            tileDef.spriteRect = ECSEngine::Rect(sx, sy, sw, sh);
            tileDef.boundingBox = ECSEngine::Rect(bx, by, bw, bh);
            tileDef.hasCollision = true;
        }

        tileDefs[tileDef.symbol] = tileDef;
    }

    // Parse map metadata (current line is "map origin ...")
    std::istringstream mapLine(line);
    std::string mapWord, originWord, tileWord, sizeWord;
    float originX, originY, tileWidth, tileHeight;
    int gridWidth, gridHeight;

    mapLine >> mapWord >> originWord >> originX >> originY
            >> tileWord >> tileWidth >> tileHeight
            >> sizeWord >> gridWidth >> gridHeight;

    if (mapWord != "map" || originWord != "origin" || tileWord != "tile" || sizeWord != "size") {
        std::cerr << "Error: Invalid map metadata in " << mapFilePath << std::endl;
        return layerData;
    }

    // Parse and create entities from the tile grid
    std::vector<std::string> gridLines;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            gridLines.push_back(line);
        }
    }

    // Create entities for each tile
    for (int row = 0; row < gridHeight && row < static_cast<int>(gridLines.size()); ++row) {
        const std::string& gridLine = gridLines[row];

        for (int col = 0; col < gridWidth && col < static_cast<int>(gridLine.length()); ++col) {
            char symbol = gridLine[col];

            // Skip empty tiles (period or space)
            if (symbol == '.' || symbol == ' ') {
                continue;
            }

            // Look up tile definition
            auto it = tileDefs.find(symbol);
            if (it == tileDefs.end()) {
                std::cerr << "Warning: Undefined tile symbol '" << symbol << "' at ("
                          << col << ", " << row << ")" << std::endl;
                continue;
            }

            const TileDef& tileDef = it->second;

            // Calculate world position (bottom-left of tile)
            float worldX = originX + col * tileWidth;
            float worldY = originY + (row + 1) * tileHeight;

            // Create entity -- assumes sky and wor
            std::string entityName = std::string("tile_") + symbol + "_"
                                   + std::to_string(col) + "_" + std::to_string(row);
            ECSEngine::EntityID entity = entityManager.CreateEntity(entityName);

            // Add location component
            entityManager.AddComponent(entity, ECSEngine::LocationComponent(worldX, worldY));

            // Register sprite and add sprite component
            std::string fullTexturePath = resourcePath + tileDef.texturePath;
            ECSEngine::SpriteID spriteID = spriteManager.RegisterTexture(fullTexturePath, tileDef.spriteRect);

            // Sprite rect is relative to entity location (location = bottom-left)
            ECSEngine::Rect spriteDrawRect(
                ECSEngine::Point2D(0.0f, -tileHeight),
                static_cast<int>(tileWidth),
                static_cast<int>(tileHeight));
            entityManager.AddComponent(entity, ECSEngine::SpriteComponent(spriteID, spriteDrawRect, true));

            // Add collision component if dictionary type 2 and not in non-collidable set
            if (tileDef.hasCollision && nonCollidableSymbols.find(symbol) == nonCollidableSymbols.end()) {
                ECSEngine::Point2D bboxTopLeft = tileDef.boundingBox.topLeft;
                bboxTopLeft.y -= tileHeight; // convert from top-left-based to bottom-left-based offset
                ECSEngine::Rect collisionRect(
                    bboxTopLeft,
                    tileDef.boundingBox.width,
                    tileDef.boundingBox.height);
                entityManager.AddComponent(entity, ECSEngine::CollisionComponent(collisionRect, true));
            }

            // Track entity by symbol
            layerData.AddEntity(symbol, entity);
        }
    }

    file.close();

    std::cout << "Loaded map layer from " << mapFilePath << " (dictionary " << dictType << ")" << std::endl;

    return layerData;
}
