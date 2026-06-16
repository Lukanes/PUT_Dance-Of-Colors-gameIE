#pragma once
#include <SFML/Graphics.hpp>
#include "movement.h"
#include "sprites/CloakMesh.h"

class Player {
public:
    Player(float startX, float startY);

    void update(float deltaTime, float screenWidth, float screenHeight);
    void draw(sf::RenderWindow& window);

    int getHealth() const;
    void setHealth(int newHealth);

private:
    sf::RectangleShape shape;
    int health;
    PlayerMovement movementHandler;

    CloakMesh cloak;
    sf::Clock gameClock;
};