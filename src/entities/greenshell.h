#pragma once

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "entities/item.h"
#include "map/map.h"

class GreenShell : public Item {
   private:
    static constexpr const int NUM_LIVES = 6;
    static constexpr const int NUM_MARCHES = 10;
    static constexpr const float SPEED = 150.0f;
    static constexpr const float HITBOX_RADIUS = 4.0f;
    static sf::Texture assetShell;
    sf::Vector2f speed;
    int lives;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    GreenShell(const sf::Vector2f &_position, const float forwardAngle,
               const bool forwardThrow);

    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float distance2) override;
};