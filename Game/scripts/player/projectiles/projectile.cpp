#include "projectile.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

PlayerProjectile::PlayerProjectile(float startX, float startY, float dirX, float dirY, int projType, float charge, const sf::Texture* tex) : sprite(*tex) {
    type = projType;
    active = true;
    timeAlive = 0.f;
    lifetime = 0.f;
    speed = 0.f;
    currentAngle = 0.f;
    airSpinSpeed = 0.f;
    airExpansionSpeed = 0.f;
    animTimer = 0.f;
    seqIndex = 0;

    if (type == 0) {
        frameWidth = 52; frameHeight = 48; frameDuration = 0.08f;
        frameSequence = { 0, 1, 2, 3, 4, 5, 4, 3 };
        loopStartIndex = 2;
    }
    else if (type == 1) {
        frameWidth = 52; frameHeight = 29; frameDuration = 0.1f;
        frameSequence = { 0, 1, 2 };
        loopStartIndex = 0;
    }
    else if (type == 2) {
        frameWidth = 65; frameHeight = 40; frameDuration = 0.08f;
        frameSequence = { 0, 1, 2, 3, 4 };
        loopStartIndex = 1;
    }
    else if (type == 3) {
        frameWidth = tex->getSize().x; frameHeight = tex->getSize().y; frameDuration = 1.0f;
        frameSequence = { 0 };
        loopStartIndex = 0;
    }

    int startFrame = frameSequence[0];
    sprite.setTextureRect(sf::IntRect({ startFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f));
    sprite.setPosition(sf::Vector2f(startX, startY));
    sprite.setColor(sf::Color::White);

    float desiredSize = 150.f;
    float scale = desiredSize / static_cast<float>(frameWidth);
    sprite.setScale(sf::Vector2f(scale, scale));

    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length != 0) {
        dirX /= length;
        dirY /= length;
    }
    else {
        dirX = 1.f;
        dirY = 0.f;
    }

    initialDir = sf::Vector2f(startX, startY);

    if (type == 0) {
        speed = 1200.f;
        lifetime = 6.0f;
        currentAngle = std::atan2(dirY, dirX);
        velocity = sf::Vector2f(dirX * speed, dirY * speed);
    }
    else if (type == 1) {
        speed = 1200.f;
        lifetime = 2.0f;
        velocity = sf::Vector2f(dirX * speed, dirY * speed);
        float angle = std::atan2(dirY, dirX) * 180.f / 3.14159f;
        sprite.setRotation(sf::degrees(angle));
    }
    else if (type == 2) {
        float randFactor = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        airSpinSpeed = 4.0f + randFactor * 5.0f;
        airExpansionSpeed = 300.f + randFactor * 300.f;
        speed = airExpansionSpeed;
        lifetime = 3.0f;
        currentAngle = 0.f;
    }
    else if (type == 3) {
        speed = 1500.f;
        velocity = sf::Vector2f(dirX * speed, dirY * speed - 600.f);
        lifetime = 4.0f;
    }
}

void PlayerProjectile::update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) {
    timeAlive += deltaTime;
    animTimer += deltaTime;

    if (animTimer >= frameDuration) {
        animTimer -= frameDuration;
        seqIndex++;
        if (seqIndex >= frameSequence.size()) {
            seqIndex = loopStartIndex;
        }
    }
    int currentFrame = frameSequence[seqIndex];
    sprite.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));

    if (type == 0) {
        float length = std::sqrt(aimX * aimX + aimY * aimY);
        if (length != 0) { aimX /= length; aimY /= length; }
        else { aimX = 1.f; aimY = 0.f; }

        float targetAngle = std::atan2(aimY, aimX);
        float diff = targetAngle - currentAngle;

        while (diff < -3.14159265f) diff += 6.28318530f;
        while (diff > 3.14159265f) diff -= 6.28318530f;

        currentAngle += diff * 6.0f * deltaTime;
        sprite.setRotation(sf::degrees(currentAngle * 180.f / 3.14159f));

        velocity.x = std::cos(currentAngle) * speed;
        velocity.y = std::sin(currentAngle) * speed;

        float waveOffset = std::sin(timeAlive * 35.f) * 10.f;
        sf::Vector2f perp(-velocity.y, velocity.x);
        float perpLength = std::sqrt(perp.x * perp.x + perp.y * perp.y);
        if (perpLength != 0) { perp.x /= perpLength; perp.y /= perpLength; }

        sprite.move(velocity * deltaTime + sf::Vector2f(perp.x * waveOffset * deltaTime, perp.y * waveOffset * deltaTime));
    }
    else if (type == 2) {
        currentAngle += airSpinSpeed * deltaTime;
        float radius = timeAlive * speed;
        float newX = initialDir.x + std::cos(currentAngle) * radius;
        float newY = initialDir.y - std::sin(currentAngle) * radius;
        sprite.setPosition(sf::Vector2f(newX, newY));
        sprite.rotate(sf::degrees(800.f * deltaTime));
    }
    else if (type == 3) {
        velocity.y += 1800.f * deltaTime;
        sprite.move(velocity * deltaTime);
        sprite.rotate(sf::degrees(150.f * deltaTime));
    }
    else {
        sprite.move(velocity * deltaTime);
    }

    lifetime -= deltaTime;
    if (lifetime <= 0.f) active = false;
}

void PlayerProjectile::draw(sf::RenderWindow& window) {
    if (active) window.draw(sprite);
}

bool PlayerProjectile::isActive() const { return active; }
int PlayerProjectile::getType() const { return type; }

int PlayerProjectile::getDamage() const {
    if (type == 0) return 5;
    if (type == 1) return 50;
    if (type == 2) return 10;
    if (type == 3) return 35;
    return 0;
}

bool PlayerProjectile::checkCollision(const sf::FloatRect& targetBounds) const {
    if (!active) return false;
    return sprite.getGlobalBounds().findIntersection(targetBounds).has_value();
}

void PlayerProjectile::deactivate() {
    active = false;
}

sf::Vector2f PlayerProjectile::getPosition() const {
    return sprite.getPosition();
}