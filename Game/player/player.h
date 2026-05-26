#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player(float startX, float startY);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    int getHealth() const;
    void setHealth(int newHealth);

private:
    void handleInput(float deltaTime);

    sf::RectangleShape shape;
    int health;
    float movementSpeed;
};