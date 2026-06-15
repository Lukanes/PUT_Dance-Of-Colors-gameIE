#include "area.h"

Area::Area(float width, float height) : screenWidth(width), screenHeight(height) {}

bool Area::loadResources() {
    if (!bgTextures[0].loadFromFile("assets/fire_background.png")) return false;
    if (!bgTextures[1].loadFromFile("assets/water_background.png")) return false;
    if (!bgTextures[2].loadFromFile("assets/ground_background.png")) return false;
    if (!bgTextures[3].loadFromFile("assets/air_background.png")) return false;
    return true;
}

void Area::draw(sf::RenderWindow& window, int currentPhase) {
    if (currentPhase >= 0 && currentPhase < 4) {
        sf::Sprite bgSprite(bgTextures[currentPhase]);
        sf::Vector2u texSize = bgTextures[currentPhase].getSize();

        float scaleX = screenWidth / static_cast<float>(texSize.x);
        float scaleY = screenHeight / static_cast<float>(texSize.y);

        bgSprite.setScale(sf::Vector2f(scaleX, scaleY));
        window.draw(bgSprite);
    }
}
bool Area::enforceBoundaries(sf::RectangleShape& shape, sf::Vector2f& velocity, float screenWidth, float screenHeight) {
    bool hitFloor = false;
    sf::Vector2f pos = shape.getPosition();
    sf::FloatRect bounds = shape.getGlobalBounds();

    if (pos.x < 0.f) {
        pos.x = 0.f;
        velocity.x = 0.f;
    }
    else if (pos.x + bounds.size.x > screenWidth) {
        pos.x = screenWidth - bounds.size.x;
        velocity.x = 0.f;
    }

    if (pos.y < 0.f) {
        pos.y = 0.f;
        velocity.y = 0.f;
    }
    else if (pos.y + bounds.size.y > screenHeight) {
        pos.y = screenHeight - bounds.size.y;
        velocity.y = 0.f;
        hitFloor = true;
    }

    shape.setPosition(pos);
    return hitFloor;
}