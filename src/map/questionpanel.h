#pragma once

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

#include "entities/driver.h"
#include "gui/gui.h"
#include "map/floorobject.h"
#include "map/map.h"

class QuestionPanel : public FloorObject {
   private:
    static constexpr const int NUM_ITEMS_ARRAY = 16;
    using ItemArray = std::array<PowerUps, NUM_ITEMS_ARRAY>;
    static ItemArray ITEMS_1, ITEMS_24, ITEMS_58;  // 1st|2nd-4th|5th-8th
    static sf::Image assetsActive[(int)FloorObjectOrientation::__COUNT],
        assetsInactive[(int)FloorObjectOrientation::__COUNT];

    virtual FloorObjectState getInitialState() const override {
        return FloorObjectState::ACTIVE;
    }

   public:
    static void loadAssets(const std::string &assetName, sf::IntRect activeRect,
                           sf::IntRect inactiveRect);

    QuestionPanel(const sf::Vector2f &topLeftPixels,
                  const FloorObjectOrientation _orientation);

    virtual void applyChanges() const override;

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
    virtual MapLand getCurrentLand() const override;
};