#include "area.h"

void Area::enforceBoundaries(sf::RectangleShape& shape, sf::Vector2f& velocity, float screenWidth, float screenHeight) {
    sf::Vector2f position = shape.getPosition();
    sf::FloatRect bounds = shape.getGlobalBounds();

    if (position.y + bounds.size.y >= screenHeight) {
        position.y = screenHeight - bounds.size.y;
        velocity.y = 0.f;
    }
    else if (position.y <= 0.f) {
        position.y = 0.f;
        velocity.y = 0.f;
    }

    if (position.x + bounds.size.x >= screenWidth) {
        position.x = screenWidth - bounds.size.x;
        velocity.x = 0.f;
    }
    else if (position.x <= 0.f) {
        position.x = 0.f;
        velocity.x = 0.f;
    }

    shape.setPosition(position);
}