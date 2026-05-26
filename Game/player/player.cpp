#include "player.h"

Player::Player(float startX, float startY) {
    shape.setSize(sf::Vector2f(10.f, 20.f));
    shape.setFillColor(sf::Color(0, 0, 0));
    shape.setPosition(sf::Vector2f(startX, startY));

    health = 100;
    movementSpeed = 300.f;
}

void Player::update(float deltaTime) {
    handleInput(deltaTime);
}

void Player::handleInput(float deltaTime) {
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movement.y -= movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movement.y += movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += movementSpeed * deltaTime;
    }

    shape.move(movement);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

int Player::getHealth() const {
    return health;
}

void Player::setHealth(int newHealth) {
    health = newHealth;
}