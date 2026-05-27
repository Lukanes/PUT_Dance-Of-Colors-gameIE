#include "player.h"
#include "area.h"
#include <cmath>

Player::Player(float startX, float startY) : movementHandler(400.f, 600.f, 1200.f, 800.f) {
    shape.setSize(sf::Vector2f(100.f, 200.f));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(1.f);
    shape.setPosition(sf::Vector2f(startX, startY));

    health = 7;

    // Inicjalizacja peleryny
    cloak.initialize(32, 90.f, 170.f);  // wider than before

    // In update(), position it slightly behind center:
}

void Player::update(float deltaTime, float screenWidth, float screenHeight) {
    movementHandler.handleInput(deltaTime);
    movementHandler.applyPhysics(deltaTime);
    movementHandler.updatePosition(shape, deltaTime);

    sf::Vector2f currentVelocity = movementHandler.getVelocity();
    Area::enforceBoundaries(shape, currentVelocity, screenWidth, screenHeight);
    movementHandler.setVelocity(currentVelocity);

    // Przekazujemy deltaTime oraz prędkość do peleryny, aby animacja była zależna od ruchu
    cloak.update(deltaTime, currentVelocity);
    

    sf::Vector2f playerPos = shape.getPosition();
    
    // Ustawienie pozycji peleryny (np. na środku gracza)
    float cloakX = playerPos.x + 5.f;
    float cloakY = playerPos.y + 8.f;
    
    cloak.setPosition(sf::Vector2f(cloakX, cloakY));
}

void Player::draw(sf::RenderWindow& window) {
    // Rysujemy pelerynę przed graczem, aby sprawiała wrażenie "założonej"
    window.draw(cloak);
    window.draw(shape);
}

int Player::getHealth() const {
    return health;
}

void Player::setHealth(int newHealth) {
    health = newHealth;
}