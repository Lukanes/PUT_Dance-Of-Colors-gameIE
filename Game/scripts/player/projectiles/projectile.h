#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class PlayerProjectile {
public:
    PlayerProjectile(float startX, float startY, float dirX, float dirY, int projType, float charge, const sf::Texture* tex);
    void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY);
    void draw(sf::RenderWindow& window);
    bool isActive() const;
    sf::Vector2f getPosition() const;
    int getType() const;
    int getDamage() const;
    bool checkCollision(const sf::FloatRect& targetBounds) const;
    void deactivate();

private:
    sf::Sprite sprite;

    int frameWidth;
    int frameHeight;
    float animTimer;
    float frameDuration;
    int seqIndex;
    int loopStartIndex;
    std::vector<int> frameSequence;

    sf::Vector2f velocity;
    sf::Vector2f initialDir;
    float speed;
    float lifetime;
    float timeAlive;
    bool active;
    int type;
    float currentAngle;
    float airSpinSpeed;
    float airExpansionSpeed;
};