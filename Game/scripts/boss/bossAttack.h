#pragma once
#include <SFML/Graphics.hpp>

struct BossAttack {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime = 0.f;
    bool active = false;
    int type = 0;
};