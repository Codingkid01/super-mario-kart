#include "racemanager.h"

// #define NO_ANIMATIONS  // remove race begin/end animations from the game

void StateRaceManager::updatePositions() {
    for (uint i = 0; i < positions.size(); i++) {
        sf::Vector2f pos = Map::getPlayerInitialPosition(i + 1);
        positions[i]->setPositionAndReset(
            sf::Vector2f(pos.x / MAP_ASSETS_WIDTH, pos.y / MAP_ASSETS_HEIGHT));
        positions[i]->posAngle = M_PI_2 * -1.0f;
        positions[i]->speedForward = 0.0f;
        positions[i]->speedTurn = 0.0f;
    }
}

void StateRaceManager::setPlayer() {
    constexpr int count = (int)MenuPlayer::__COUNT;
    for (int i = 0; i < count; i++) {
#ifdef NO_ANIMATIONS
        drivers[i]->controlType = DriverControlType::DISABLED;
#else
        drivers[i]->controlType = DriverControlType::AI_GRADIENT;
#endif
    }
    // move selected player to last position
    std::swap(positions[(int)selectedPlayer], positions[count - 1]);
    std::random_shuffle(positions.begin(), positions.begin() + (count - 1));
}

void StateRaceManager::init(const float _speedMultiplier,
                            const RaceCircuit _circuit) {
    speedMultiplier = _speedMultiplier;
    currentCircuit = _circuit;
    for (uint i = 0; i < (uint)MenuPlayer::__COUNT; i++) {
        DriverPtr driver(new Driver(
            DRIVER_ASSET_NAMES[i].c_str(), sf::Vector2f(0.0f, 0.0f),
            M_PI_2 * -1.0f, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
            DriverControlType::DISABLED, *DRIVER_PROPERTIES[i], MenuPlayer(i)));
        drivers[i] = driver;
        positions[i] = driver.get();
        grandPrixRanking[i] = std::make_pair(driver.get(), 0);
    };
    currentState = RaceState::NO_PLAYER;
}

void StateRaceManager::update(const sf::Time &) {
    switch (currentState) {
        case RaceState::NO_PLAYER:
            game.pushState(
                StatePtr(new StatePlayerSelection(game, selectedPlayer)));
            currentState = RaceState::YES_PLAYER;
            break;
        case RaceState::YES_PLAYER:
            setPlayer();
            currentState = RaceState::RACING;
            break;
        case RaceState::RACING:
            int i = (int)currentCircuit;
            currentCircuit = RaceCircuit(i + 1);
            drivers[(uint)selectedPlayer]->controlType =
                DriverControlType::PLAYER;
            Map::loadCourse(CIRCUIT_ASSET_NAMES[i]);
            updatePositions();
            uint currentPlayerPosition;
            for (uint i = 0; i < positions.size(); i++) {
                if (positions[i]->getPj() == selectedPlayer) {
                    currentPlayerPosition = i;
                    break;
                }
            }
#ifndef NO_ANIMATIONS
            game.pushState(
                StatePtr(new StateRaceEnd(game, drivers[(uint)selectedPlayer],
                                          drivers, selectedPlayer, positions)));
#endif
            game.pushState(StatePtr(new StateRace(
                game, drivers[(uint)selectedPlayer], drivers, positions)));
#ifndef NO_ANIMATIONS
            game.pushState(StatePtr(new StateRaceStart(
                game, drivers,
                Map::getPlayerInitialPosition(currentPlayerPosition + 1))));
#endif
            // TODO add points to grandprixranking according to positions array
            // TODO maybe add another state for points animation
            break;
    }
}