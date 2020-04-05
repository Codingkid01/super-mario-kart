#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#define M_PI 3.14159265358979323846 /* pi */

class Map;

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

#include "entities/collisionhashmap.h"
#include "entities/driver.h"
#include "entities/enums.h"
#include "entities/pipe.h"
#include "entities/thwomp.h"
#include "entities/wallobject.h"
#include "game.h"
#include "map/coin.h"
#include "map/enums.h"
#include "map/floorobject.h"
#include "map/oilslick.h"
#include "map/questionpanel.h"
#include "map/zipper.h"

class Map {
   private:
    // Singleton instance
    static Map instance;

    // Frustum configuration
    static constexpr float MODE7_FOV_HALF = 0.5f;
    static constexpr float MODE7_CLIP_NEAR = 0.0005f;
    static constexpr float MODE7_CLIP_FAR = 0.045f;

    static constexpr float MINIMAP_POS_DISTANCE = 6.5f;
    static constexpr float MINIMAP_FOV_HALF = M_PI / 32.0f;

    // Coordinates for the left corners of the map
    static constexpr float MINIMAP_BOTTOM_X = 20.0f / 512.0f;
    static constexpr float MINIMAP_BOTTOM_Y = 438.0f / 448.0f;
    static constexpr float MINIMAP_TOP_X = 57.0f / 512.0f;
    static constexpr float MINIMAP_TOP_Y = 252.0f / 448.0f;

   public:
    // Generate image as window width * (height * height_pct) rectangle
    static constexpr float SKY_SCALE = 2.0f;
    static constexpr float SKY_CUT_PCT = 20.0f / 32.0f;
    static constexpr float SKY_HEIGHT_PCT = 2.0f / 22.4f;
    static constexpr float CIRCUIT_HEIGHT_PCT = 9.2f / 22.4f;
    static constexpr float MINIMAP_HEIGHT_PCT = 11.2f / 22.4f;
    // Driver to Camera distance (in image pixels)
    static constexpr float CAM_2_PLAYER_DST = 46.0f;

   private:
    // Image with specified maps
    const sf::RenderWindow *gameWindow;
    // Assets read directly from files
    sf::Image assetCourse, assetSkyBack, assetSkyFront, assetEdges;
    // Assets generated from other assets
    sf::Image assetObjects;  // course generated from assetCourse
    sf::Image assetMinimap;  // minimap generated from assetObjects
    // Current floor/wall objects in play
    std::vector<FloorObjectPtr> floorObjects;
    std::vector<FloorObjectPtr> specialFloorObjects;
    std::vector<WallObjectPtr> wallObjects;
    static inline sf::Color sampleAsset(const sf::Image &asset,
                                        const sf::Vector2f &sample) {
        sf::Vector2u size = asset.getSize();
        return asset.getPixel(sample.x * size.x, sample.y * size.y);
    }

    MapLandMatrix landTiles;

    // Aux data
    sf::FloatRect goal;
    int nCp;
    std::list<sf::FloatRect> checkpoints;
    int aiFarVision;

    // Music
    // sf::Sound sounds;
    // sf::Music music;

    const sf::Color sampleMap(const sf::Vector2f &sample);

    const sf::Image mode7(const sf::Vector2f &position, const float angle,
                          const float fovHalf, const float clipNear,
                          const float clipFar, const sf::Vector2u &size,
                          const bool perspective);

   public:
    static void setGameWindow(const Game &game);

    // Get a point in the map
    static inline MapLand getLand(const sf::Vector2f &position) {
        // position in 0-1 range
        return instance.landTiles[int(position.y * MAP_TILES_HEIGHT)]
                                 [int(position.x * MAP_TILES_WIDTH)];
    }

    // Set a point in the map
    static inline void setLand(const sf::Vector2f &position,
                               const sf::Vector2f &size, MapLand land) {
        // position in 0-1 range
        // size in px
        for (int y = 0; y < size.y / MAP_TILE_SIZE; y++) {
            for (int x = 0; x < size.x / MAP_TILE_SIZE; x++) {
                instance.landTiles[int(position.y * MAP_TILES_HEIGHT + y)]
                                  [int(position.x * MAP_TILES_WIDTH + x)] =
                    land;
            }
        }
    }

    // Check if in meta
    static inline bool inGoal(const sf::Vector2f &ppos) {
        return instance.goal.contains(ppos);
    }

    // Num of checkpoints
    static inline int numCheckpoints() { return instance.nCp; }

    // Get checkpoints
    static inline std::list<sf::FloatRect> getCheckpoints() {
        return instance.checkpoints;
    }

    // Load all map resources so all interactions
    static bool loadCourse(const std::string &course);

    // Start map
    static void startCourse();

    // AI-specific loading (gradient)
    static void loadAI();

    // Special course-dependent AI variables
    static int getCurrentMapAIFarVision();

    // make one driver interact with a floor object
    static void collideWithSpecialFloorObject(const DriverPtr &driver);

    // register wall objects for collision detection
    static void registerWallObjects();

    // loop through all the wall objects
    static void updateObjects(const sf::Time &deltaTime);

    // Generate minimap image
    static void updateMinimap();

    // replaces a section of the course asset
    static void updateAssetCourse(const sf::Image &newAsset,
                                  const sf::Vector2f &topLeftPixels);

    // Sky rectangle image with parallax effect
    static void skyTextures(const DriverPtr &player, sf::Texture &skyBack,
                            sf::Texture &skyFront);

    // Mode 7 perspective image with given position and angle
    static void circuitTexture(const DriverPtr &player,
                               sf::Texture &circuitTexture);

    // Mode 7 minimap on lower half of the screen
    static void mapTexture(sf::Texture &mapTexture);

    // Convert player map coordinates (in 0-1 range)
    // to minimap coordinates (in a window with 8:7 resolution)
    static sf::Vector2f mapCoordinates(sf::Vector2f &position);

    // Convert circuit coordinates to screen coordinates
    // returns true if coords are within the screen, false if not
    static bool mapToScreen(const DriverPtr &player,
                            const sf::Vector2f &mapCoords,
                            sf::Vector2f &screenCoords, float &z);

    // All 2D sprite map objects (not counting drivers)
    static void getWallDrawables(
        const sf::RenderTarget &window, const DriverPtr &player,
        std::vector<std::pair<float, sf::Sprite *>> &drawables);

    // All 2D sprite map objects (only drivers)
    static void getDriverDrawables(
        const sf::RenderTarget &window, const DriverPtr &player,
        const std::vector<DriverPtr> &drivers,
        std::vector<std::pair<float, sf::Sprite *>> &drawables);

    // Get the initial position (in pixels) from a player that will start
    // the race at the i-th position, 1-indexed. So pole position is 1.
    static sf::Vector2f getPlayerInitialPosition(int position);
};