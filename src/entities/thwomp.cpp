#include "thwomp.h"

sf::Texture Thwomp::assetNormal;
sf::Texture Thwomp::assetSuper;

void Thwomp::loadAssets(const std::string &assetName,
                        const sf::IntRect &roiNormal,
                        const sf::IntRect &roiSuper) {
    assetNormal.loadFromFile(assetName, roiNormal);
    assetSuper.loadFromFile(assetName, roiSuper);
}

Thwomp::Thwomp(const sf::Vector2f &position, bool _isSuper)
    : WallObject(position, 2.5f, 2.0f, MAX_HEIGHT, MAP_ASSETS_WIDTH,
                 MAP_ASSETS_HEIGHT),
      sprite(_isSuper ? assetSuper : assetNormal),
      currentState(State::UP),
      currentTime(0.0f),
      isSuper(_isSuper) {
    sf::Vector2u size = sprite.getTexture()->getSize();
    sprite.setOrigin(size.x / 2.0f, size.y);
}

void Thwomp::update(const sf::Time &deltaTime) {
    currentTime += deltaTime.asSeconds();
    switch (currentState) {
        case State::UP:
            if (currentTime >= 0.0f) {
                currentState = State::GOING_DOWN;
                currentTime = 0.0f;
            }
            break;
        case State::GOING_DOWN:
            currentTime = fminf(currentTime, 0.15f);
            height = (0.15f - currentTime) * 4.0f * MAX_HEIGHT;
            if (currentTime == 0.15f) {
                currentState = State::DOWN;
                currentTime = 0.0f;
            }
            break;
        case State::DOWN:
            currentTime = fminf(currentTime, 6.0f);
            if (currentTime == 6.0f) {
                currentState = State::GOING_UP;
                currentTime = 0.0f;
            }
            break;
        case State::GOING_UP:
            currentTime = fminf(currentTime, 2.0f);
            height = currentTime * MAX_HEIGHT / 2.0f;
            if (currentTime == 2.0f) {
                currentState = State::UP;
                currentTime = ((rand() % 7) + 1) * -1;
                if (isSuper) {
                    currentTime *= 0.7f;
                }
            }
            break;
    }
}

bool Thwomp::solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                            const sf::Vector2f &otherPos, const float,
                            const float distance2) {
    if (height > STOMP_HEIGHT) {
        // too high to collide
        return false;
    }
    if (distance2 > hitboxRadius * hitboxRadius ||
        currentState != State::GOING_DOWN) {
        // hit from outside, doesn't stomp
        return WallObject::defaultSolveCollision(data, otherSpeed, otherPos,
                                                 position, distance2);
    }
    // only stop if state is going_down and dist2 is inside the thwomp's radius
    // stomp the user: remove all speed
    data = CollisionData(sf::Vector2f(0.0f, 0.0f), 0.0f);
    return true;
}