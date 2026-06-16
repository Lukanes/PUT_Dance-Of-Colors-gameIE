#pragma once
#include <SFML/Graphics.hpp>

class Area {
private:
    sf::Texture bgTextures[4];
    float screenWidth;
    float screenHeight;

public:
    Area(float width, float height);
    bool loadResources();
    void update(int currentPhase);
    void draw(sf::RenderWindow& window, int currentPhase);
    static bool enforceBoundaries(sf::RectangleShape& shape, sf::Vector2f& velocity, float screenWidth, float screenHeight);
};