#pragma once
#include <SFML/Graphics.hpp>
#include "clock.h"

class Interface {
private:
    sf::Font font;
    sf::Text clockText;
    sf::Text scoreText;

public:
    Interface();
    bool loadResources();
    void update(const GameClock& gameClock, int score);
    void draw(sf::RenderWindow& window);
};