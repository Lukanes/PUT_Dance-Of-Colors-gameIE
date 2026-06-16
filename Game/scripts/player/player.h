#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include "movement/movement.h"
#include "projectiles/projectile.h"
#include "sprites/CloakMesh.h"

class Player {
public:
    Player(float startX, float startY);
    void update(float deltaTime, float screenWidth, float screenHeight, const std::vector<sf::FloatRect>& platforms);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    int getHealth() const;
    void setHealth(int newHealth);
    float getMaxHealth() const { return 10.f; }

    std::vector<std::unique_ptr<Projectile>>& getProjectiles();

    sf::Vector2f getPosition() const;
    void resetPositionForPhase(int phase, float screenWidth, float screenHeight);
    void reset(float startX, float startY);
    void playSound(int index);

private:
    sf::RectangleShape shape;

    sf::Texture texProjFire;
    sf::Texture texProjWater;
    sf::Texture texProjEarth;
    sf::Texture texProjAir;

    sf::Sprite headSprite;
    float hoverOffset;
    float headAnimTimer;
    int headSeqIndex;

    sf::SoundBuffer soundBuffers[5];
    std::vector<sf::Sound> sounds;

    PlayerMovement movementHandler;
    CloakMesh cloak;
    int health;

    std::vector<std::unique_ptr<Projectile>> projectiles;

    int currentElementType;
    std::vector<int> elementCycle;
    int currentElementIndex;

    float chargeTimer;
    bool isFullyCharged;
    bool prevZPressed;
    bool prevXPressed;
    bool prevCPressed;

    float totalTime;
    float airCooldownTimer;
};