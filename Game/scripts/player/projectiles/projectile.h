#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Projectile {
protected:
    sf::Sprite sprite;
    float timeAlive;
    float lifetime;
    bool active;
    int type;


    int frameWidth;
    int frameHeight;
    float animTimer;
    float frameDuration;
    int seqIndex;
    int loopStartIndex;
    std::vector<int> frameSequence;

    void updateAnimation(float deltaTime);

public:
    Projectile(float startX, float startY, int projType, const sf::Texture* tex);
    virtual ~Projectile() = default;

    virtual void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) = 0;

    void draw(sf::RenderWindow& window);
    bool isActive() const;
    sf::Vector2f getPosition() const;
    int getType() const;
    virtual int getDamage() const = 0;
    sf::FloatRect getBounds() const;
    bool checkCollision(const sf::FloatRect& targetBounds) const;
    void deactivate();
};

class WaterProjectile : public Projectile {
private:
    float speed;
    sf::Vector2f velocity;
    float currentAngle;
public:
    WaterProjectile(float startX, float startY, float dirX, float dirY, float charge, const sf::Texture* tex);
    void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) override;
    int getDamage() const override { return 5; }
};

class FireProjectile : public Projectile {
private:
    float speed;
    sf::Vector2f velocity;
public:
    FireProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex);
    void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) override;
    int getDamage() const override { return 50; }
};

class AirProjectile : public Projectile {
private:
    float speed;
    sf::Vector2f velocity;
public:
    AirProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex);
    void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) override;
    int getDamage() const override { return 20; }
};

class EarthProjectile : public Projectile {
private:
    sf::Vector2f velocity;
public:
    EarthProjectile(float startX, float startY, float dirX, float dirY, const sf::Texture* tex);
    void update(float deltaTime, sf::Vector2f playerPos, float aimX, float aimY) override;
    int getDamage() const override { return 40; }
};