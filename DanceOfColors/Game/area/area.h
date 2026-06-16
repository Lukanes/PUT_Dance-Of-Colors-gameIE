#pragma once
#include <SFML/Graphics.hpp>

namespace Area {
    void enforceBoundaries(sf::RectangleShape& shape, sf::Vector2f& velocity, float screenWidth, float screenHeight);
}