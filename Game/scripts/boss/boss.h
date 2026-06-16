#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "bossAttack.h"
#include "../animation/animation.h"

class Boss {
private:
    sf::Texture texWaterBubble;
    sf::Texture texWaterKnife;
    sf::Texture texWaterGeyser;
    sf::Texture texAirSlash;
    sf::Texture texAirDrop;
    sf::Texture texAirShot;
    sf::Texture texFireDrop;
    sf::Texture texFireMeteor;
    sf::Texture texFireFloor;
    sf::Texture texEarthRock;
    sf::Texture texEarthLaser;
    sf::Texture texEarthShot;
    sf::Texture texPlatform;
    sf::Texture texWaterFloor;

    sf::Texture texBossFire;
    sf::Texture texBossWater;
    sf::Texture texBossAir;
    sf::Texture texBossGround;
    sf::Sprite bossSprite;
    Animation bossAnim;

    sf::SoundBuffer soundBuffers[5];
    std::vector<sf::Sound> sounds;

    sf::RectangleShape shape;
    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBarFg;
    sf::RectangleShape waterFloor;
    sf::RectangleShape earthWall;

    float health;
    float maxHealth;
    float phaseThreshold;
    int currentPhase;

    float stateTimer;
    float attackTimer;
    float subStateTimer;
    int attackState;
    float invulnTimer;
    bool isVulnerable;
    float width;
    float height;
    float aggressionMultiplier;

    float attackWeights[4][6];
    sf::Vector2f savedTargetPos;
    sf::Vector2f dashStartPos;

    std::vector<sf::RectangleShape> platforms;
    std::vector<BossAttack> attacks;
    std::vector<BossAttack> floorFlames;
    std::vector<sf::RectangleShape> telegraphs;

    void setupPhase();
    int chooseAttack(int phase, int numAttacks);

public:
    Boss(float screenWidth, float screenHeight);
    void update(float deltaTime, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window);
    void reset(float screenWidth, float screenHeight);
    void nextPhase();
    void spawnAttack(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f size, float life, sf::Color color, const sf::Texture* tex = nullptr, int type = 0);
    void playSound(int index);

    std::vector<sf::FloatRect> getAttackBounds() const;
    std::vector<sf::FloatRect> getPlatformBounds() const;
    std::vector<BossAttack>& getAttacks();

    bool checkEarthCrush();
    void takeDamage(int dmg);

    float getX() const;
    float getY() const;
    int getCurrentPhase() const;
    bool isAlive() const;
    sf::FloatRect getBounds() const;
};