#include "projectile.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

Projectile::Projectile(float startX, float startY, int projType, const sf::Texture* tex)
    : sprite(*tex), type(projType), active(true), timeAlive(0.f), lifetime(0.f),
    animTimer(0.f), seqIndex(0), loopStartIndex(0), frameWidth(0), frameHeight(0), frameDuration(0.1f)
{
    sprite.setPosition(sf::Vector2f(startX, startY));
    sprite.setColor(sf::Color::White);
}

void Projectile::updateAnimation(float deltaTime) {
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
}

void Projectile::draw(sf::RenderWindow& window) {
    if (active) window.draw(sprite);
}

bool Projectile::isActive() const { return active; }
int Projectile::getType() const { return type; }
bool Projectile::checkCollision(const sf::FloatRect& targetBounds) const {
    if (!active) return false;
    return sprite.getGlobalBounds().findIntersection(targetBounds).has_value();
}
void Projectile::deactivate() { active = false; }
sf::Vector2f Projectile::getPosition() const { return sprite.getPosition(); }

WaterProjectile::WaterProjectile(float startX, float startY, float dirX, float dirY, float charge, const sf::Texture* tex)
    : Projectile(startX, startY, 0, tex), speed(1200.f)
{
    frameWidth = 52; frameHeight = 48; frameDuration = 0.08f;
    frameSequence = { 0, 1, 2, 3, 4, 5, 4, 3 }; loopStartIndex = 2;
    lifetime = std::max(1.0f, charge) * 0.6f;

    sprite.setTextureRect(sf::IntRect({ frameSequence[0] * frameWidth, 0 }, { frameWidth, frameHeight }));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f));

    float scale = 150.f / static_cast<float>(frameWidth);
    sprite.setScale(sf::Vector2f(dirX < 0 ? -scale : scale, scale));
    sprite.setRotation(sf::degrees(0.f));

    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length != 0) { dirX /= length; dirY /= length; }
    else { dirX = 1.f; dirY = 0.f; }

    currentAngle = std::atan2(dirY, dirX);
    velocity = sf::Vector2f(dirX * speed, dirY * speed);
}

void WaterProjectile::update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) {
    timeAlive += deltaTime;
    updateAnimation(deltaTime);

    float length = std::sqrt(aimX * aimX + aimY * aimY);
    if (length != 0) { aimX /= length; aimY /= length; }
    else { aimX = 1.f; aimY = 0.f; }

    float targetAngle = std::atan2(aimY, aimX);
    float diff = targetAngle - currentAngle;
    while (diff < -3.14159265f) diff += 6.28318530f;
    while (diff > 3.14159265f) diff -= 6.28318530f;

    currentAngle += diff * 6.0f * deltaTime;

    velocity.x = std::cos(currentAngle) * speed;
    velocity.y = std::sin(currentAngle) * speed;

    float waveOffset = std::sin(timeAlive * 35.f) * 10.f;
    sf::Vector2f perp(-velocity.y, velocity.x);
    float perpLength = std::sqrt(perp.x * perp.x + perp.y * perp.y);
    if (perpLength != 0) { perp.x /= perpLength; perp.y /= perpLength; }

    sprite.move(velocity * deltaTime + sf::Vector2f(perp.x * waveOffset * deltaTime, perp.y * waveOffset * deltaTime));

    lifetime -= deltaTime;
    if (lifetime <= 0.f) active = false;
}

FireProjectile::FireProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex)
    : Projectile(startX, startY, 1, tex), speed(1200.f)
{
    frameWidth = 52; frameHeight = 29; frameDuration = 0.1f;
    frameSequence = { 0, 1, 2 }; loopStartIndex = 0;
    lifetime = 2.0f;

    sprite.setTextureRect(sf::IntRect({ frameSequence[0] * frameWidth, 0 }, { frameWidth, frameHeight }));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f));

    float scale = 150.f / static_cast<float>(frameWidth);
    sprite.setScale(sf::Vector2f(dirX < 0 ? -scale : scale, scale));
    sprite.setRotation(sf::degrees(0.f));

    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length != 0) { dirX /= length; dirY /= length; }
    else { dirX = 1.f; dirY = 0.f; }
    velocity = sf::Vector2f(dirX * speed, dirY * speed);
}

void FireProjectile::update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) {
    timeAlive += deltaTime;
    updateAnimation(deltaTime);
    sprite.move(velocity * deltaTime);
    lifetime -= deltaTime;
    if (lifetime <= 0.f) active = false;
}

AirProjectile::AirProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex)
    : Projectile(startX, startY, 2, tex), speed(1500.f)
{
    frameWidth = 65; frameHeight = 40; frameDuration = 0.08f;
    frameSequence = { 0, 1, 2, 3, 4 }; loopStartIndex = 1;
    lifetime = 3.0f;

    sprite.setTextureRect(sf::IntRect({ frameSequence[0] * frameWidth, 0 }, { frameWidth, frameHeight }));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f));

    float scale = 150.f / static_cast<float>(frameWidth);
    sprite.setScale(sf::Vector2f(dirX < 0 ? -scale : scale, scale));
    sprite.setRotation(sf::degrees(0.f));

    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length != 0) { dirX /= length; dirY /= length; }
    else { dirX = 1.f; dirY = 0.f; }
    velocity = sf::Vector2f(dirX * speed, dirY * speed);
}

void AirProjectile::update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) {
    timeAlive += deltaTime;
    updateAnimation(deltaTime);
    sprite.move(velocity * deltaTime);
    lifetime -= deltaTime;
    if (lifetime <= 0.f) active = false;
}

EarthProjectile::EarthProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex)
    : Projectile(startX, startY, 3, tex)
{
    frameWidth = tex->getSize().x; frameHeight = tex->getSize().y; frameDuration = 1.0f;
    frameSequence = { 0 }; loopStartIndex = 0;
    lifetime = 4.0f;

    sprite.setTextureRect(sf::IntRect({ frameSequence[0] * frameWidth, 0 }, { frameWidth, frameHeight }));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f));

    float scale = 150.f / static_cast<float>(frameWidth);
    sprite.setScale(sf::Vector2f(dirX < 0 ? -scale : scale, scale));
    sprite.setRotation(sf::degrees(0.f));

    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length != 0) { dirX /= length; dirY /= length; }
    else { dirX = 1.f; dirY = 0.f; }

    float speed = 1200.f;
    velocity = sf::Vector2f(dirX * speed, dirY * speed - 800.f);
}

void EarthProjectile::update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) {
    timeAlive += deltaTime;
    updateAnimation(deltaTime);

    velocity.y += 2400.f * deltaTime;
    sprite.move(velocity * deltaTime);

    lifetime -= deltaTime;
    if (lifetime <= 0.f) active = false;
}