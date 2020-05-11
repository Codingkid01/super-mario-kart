#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>

#include "ai/gradientdescent.h"
#include "entities/collisionhashmap.h"
#include "entities/driver.h"
#include "entities/item.h"
#include "entities/lakitu.h"
#include "gui/gui.h"
#include "map/enums.h"
#include "map/map.h"
#include "states/racepause.h"
#include "states/statebase.h"

class StateRace : public State {
   private:
    static const sf::Time TIME_BETWEEN_ITEM_CHECKS;
    sf::Time nextItemCheck;

    const DriverPtr player;
    DriverArray drivers;
    DriverArray miniDrivers;
    RaceRankingArray& positions;

    bool pushedPauseThisFrame = false;
    bool raceFinished = false;
    bool driftPressed = false;

   public:
    static sf::Time currentTime;

    StateRace(Game& game, const DriverPtr& _player, const DriverArray& _drivers,
              RaceRankingArray& _positions)
        : State(game),
          player(_player),
          drivers(_drivers),
          miniDrivers(_drivers),
          positions(_positions) {
        init();
    }

    void handleEvent(const sf::Event& event) override;
    bool fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    void init();

    inline std::string string() const override { return "Race"; }
};