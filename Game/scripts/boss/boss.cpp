#include "boss.h"
#include <cmath>
#include <cstdlib>
#include <cstdint>

Boss::Boss(float screenWidth, float screenHeight) : width(screenWidth), height(screenHeight), bossSprite(texBossFire) {
    texWaterBubble.loadFromFile("assets/boss_water_bubble.png");
    texWaterKnife.loadFromFile("assets/boss_water_knife.png");
    texWaterGeyser.loadFromFile("assets/boss_water_geyser.png");
    texAirSlash.loadFromFile("assets/boss_air_slash.png");
    texAirDrop.loadFromFile("assets/boss_air_drop.png");
    texAirShot.loadFromFile("assets/boss_air_shot.png");
    texFireDrop.loadFromFile("assets/boss_fire_drop.png");
    texFireMeteor.loadFromFile("assets/boss_fire_meteor.png");
    texFireFloor.loadFromFile("assets/boss_fire_floor.png");
    texEarthRock.loadFromFile("assets/boss_earth_rock.png");
    texEarthLaser.loadFromFile("assets/boss_earth_laser.png");
    texEarthShot.loadFromFile("assets/boss_earth_shot.png");

    texPlatform.loadFromFile("assets/g.png");
    texPlatform.setRepeated(true);

    texWaterFloor.loadFromFile("assets/w.png");

    texBossFire.loadFromFile("assets/firePhase.png");
    texBossWater.loadFromFile("assets/waterPhase.png");
    texBossAir.loadFromFile("assets/airPhase.png");

    texBossGround.loadFromFile("assets/groundPhase.png");
    texBossGround.setRepeated(true);

    for (int i = 0; i < 5; ++i) sounds.emplace_back(soundBuffers[i]);

    maxHealth = 2000.f;
    health = maxHealth;
    phaseThreshold = maxHealth * 0.90f;
    invulnTimer = 0.f;
    isVulnerable = true;
    currentPhase = rand() % 4;

    stateTimer = 0.f;
    attackTimer = 0.f;
    attackState = 0;
    subStateTimer = 0.f;
    aggressionMultiplier = 1.0f;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 6; ++j) {
            attackWeights[i][j] = 100.0f;
        }
    }

    hpBarBg.setSize(sf::Vector2f(600.f, 30.f));
    hpBarBg.setFillColor(sf::Color(50, 50, 50, 200));
    hpBarBg.setOrigin(sf::Vector2f(300.f, 15.f));

    hpBarFg.setSize(sf::Vector2f(600.f, 30.f));
    hpBarFg.setFillColor(sf::Color::Red);
    hpBarFg.setOrigin(sf::Vector2f(300.f, 15.f));

    waterFloor.setSize(sf::Vector2f(screenWidth, 200.f));
    waterFloor.setTexture(&texWaterFloor);
    waterFloor.setPosition(sf::Vector2f(0.f, screenHeight - 200.f));

    earthWall.setSize(sf::Vector2f(400.f, screenHeight));
    earthWall.setTexture(&texBossGround);

    setupPhase();
}

void Boss::setupPhase() {
    platforms.clear();
    bossSprite.setScale(sf::Vector2f(1.f, 1.f));

    if (currentPhase == 0) {
        bossSprite.setTexture(texBossFire);
        bossAnim.frameSize = sf::Vector2i(texBossFire.getSize().x / 8, texBossFire.getSize().y);
        bossAnim.frameCount = 8;
        bossAnim.frameTime = 0.1f;
        bossSprite.setTextureRect(bossAnim.getRect());
        bossSprite.setOrigin(sf::Vector2f(static_cast<float>(bossAnim.frameSize.x) / 2.f, static_cast<float>(bossAnim.frameSize.y) / 2.f));
        shape.setSize(sf::Vector2f(250.f, 250.f));
        shape.setOrigin(sf::Vector2f(125.f, 125.f));
        bossSprite.setScale(sf::Vector2f(6.f, 6.f));
    }
    else if (currentPhase == 1) {
        bossSprite.setTexture(texBossWater);
        bossAnim.frameSize = sf::Vector2i(texBossWater.getSize().x / 7, texBossWater.getSize().y);
        bossAnim.frameCount = 7;
        bossAnim.frameTime = 0.15f;
        bossSprite.setTextureRect(bossAnim.getRect());
        bossSprite.setOrigin(sf::Vector2f(static_cast<float>(bossAnim.frameSize.x) / 2.f, static_cast<float>(bossAnim.frameSize.y) / 2.f));
        shape.setSize(sf::Vector2f(180.f, 180.f));
        shape.setOrigin(sf::Vector2f(90.f, 90.f));
        bossSprite.setScale(sf::Vector2f(6.f, 6.f));
    }
    else if (currentPhase == 3) {
        bossSprite.setTexture(texBossAir);
        bossAnim.frameSize = sf::Vector2i(texBossAir.getSize().x / 7, texBossAir.getSize().y);
        bossAnim.frameCount = 7;
        bossAnim.frameTime = 0.1f;
        bossSprite.setTextureRect(bossAnim.getRect());
        bossSprite.setOrigin(sf::Vector2f(static_cast<float>(bossAnim.frameSize.x) / 2.f, static_cast<float>(bossAnim.frameSize.y) / 2.f));
        shape.setSize(sf::Vector2f(400.f, 400.f));
        shape.setOrigin(sf::Vector2f(200.f, 200.f));
        bossSprite.setScale(sf::Vector2f(6.f, 6.f));
    }

    if (currentPhase == 2) {
        earthWall.setPosition(sf::Vector2f(width - 400.f, 0.f));
        earthWall.setTextureRect(sf::IntRect({ 0, 0 }, { 400, static_cast<int>(height) }));
        shape.setSize(sf::Vector2f(400.f, height));
        shape.setOrigin(sf::Vector2f(200.f, height / 2.f));
    }
    else {
        earthWall.setPosition(sf::Vector2f(-1000.f, 0.f));
        if (currentPhase != 0 && currentPhase != 1 && currentPhase != 3) {
            shape.setSize(sf::Vector2f(100.f, 150.f));
            shape.setOrigin(sf::Vector2f(50.f, 75.f));
        }
    }

    if (currentPhase == 0) {
        sf::RectangleShape p1(sf::Vector2f(600.f, 40.f)); p1.setPosition(sf::Vector2f(200.f, 700.f)); p1.setFillColor(sf::Color::White); p1.setTexture(&texPlatform); p1.setTextureRect(sf::IntRect({ 0, 0 }, { 600, 40 }));
        sf::RectangleShape p2(sf::Vector2f(600.f, 40.f)); p2.setPosition(sf::Vector2f(width - 800.f, 700.f)); p2.setFillColor(sf::Color::White); p2.setTexture(&texPlatform); p2.setTextureRect(sf::IntRect({ 0, 0 }, { 600, 40 }));
        platforms.push_back(p1); platforms.push_back(p2);
    }
    else if (currentPhase == 1) {
        sf::RectangleShape p1(sf::Vector2f(300.f, 40.f)); p1.setPosition(sf::Vector2f(800.f, 1100.f)); p1.setFillColor(sf::Color::White); p1.setTexture(&texPlatform); p1.setTextureRect(sf::IntRect({ 0, 0 }, { 300, 40 }));
        sf::RectangleShape p2(sf::Vector2f(300.f, 40.f)); p2.setPosition(sf::Vector2f(width - 1100.f, 1100.f)); p2.setFillColor(sf::Color::White); p2.setTexture(&texPlatform); p2.setTextureRect(sf::IntRect({ 0, 0 }, { 300, 40 }));
        sf::RectangleShape p3(sf::Vector2f(300.f, 40.f)); p3.setPosition(sf::Vector2f((width / 2.f) - 150.f, 1100.f)); p3.setFillColor(sf::Color::White); p3.setTexture(&texPlatform); p3.setTextureRect(sf::IntRect({ 0, 0 }, { 300, 40 }));
        platforms.push_back(p1); platforms.push_back(p2); platforms.push_back(p3);
    }
    else if (currentPhase == 3) {
        float cx = width / 2.f; float cy = height / 2.f + 100.f; float offsetX = 850.f; float offsetY = 350.f;
        sf::RectangleShape pTop(sf::Vector2f(350.f, 40.f)); pTop.setPosition(sf::Vector2f(cx - 175.f, cy - offsetY)); pTop.setFillColor(sf::Color::White); pTop.setTexture(&texPlatform); pTop.setTextureRect(sf::IntRect({ 0, 0 }, { 350, 40 }));
        sf::RectangleShape pBottom(sf::Vector2f(350.f, 40.f)); pBottom.setPosition(sf::Vector2f(cx - 175.f, cy + offsetY)); pBottom.setFillColor(sf::Color::White); pBottom.setTexture(&texPlatform); pBottom.setTextureRect(sf::IntRect({ 0, 0 }, { 350, 40 }));
        sf::RectangleShape pLeft(sf::Vector2f(350.f, 40.f)); pLeft.setPosition(sf::Vector2f(cx - offsetX - 175.f, cy)); pLeft.setFillColor(sf::Color::White); pLeft.setTexture(&texPlatform); pLeft.setTextureRect(sf::IntRect({ 0, 0 }, { 350, 40 }));
        sf::RectangleShape pRight(sf::Vector2f(350.f, 40.f)); pRight.setPosition(sf::Vector2f(cx + offsetX - 175.f, cy)); pRight.setFillColor(sf::Color::White); pRight.setTexture(&texPlatform); pRight.setTextureRect(sf::IntRect({ 0, 0 }, { 350, 40 }));
        platforms.push_back(pTop); platforms.push_back(pBottom); platforms.push_back(pLeft); platforms.push_back(pRight);
    }
}

int Boss::chooseAttack(int phase, int numAttacks) {
    float total = 0.f;
    for (int i = 0; i < numAttacks; ++i) total += attackWeights[phase][i];

    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * total;
    int chosen = 0;
    float accum = 0.f;

    for (int i = 0; i < numAttacks; ++i) {
        accum += attackWeights[phase][i];
        if (r <= accum) { chosen = i; break; }
    }

    float reduction = attackWeights[phase][chosen] / 2.f;
    attackWeights[phase][chosen] -= reduction;
    float addition = reduction / (numAttacks - 1);

    for (int i = 0; i < numAttacks; ++i) {
        if (i != chosen) attackWeights[phase][i] += addition;
    }

    return chosen;
}

void Boss::playSound(int index) {
    if (index >= 0 && index < 5) sounds[index].play();
}

void Boss::update(float deltaTime, sf::Vector2f playerPos) {
    if (currentPhase != 2) {
        bossAnim.update(deltaTime);
        bossSprite.setTextureRect(bossAnim.getRect());
    }

    stateTimer += deltaTime;
    if (attackTimer > 0.f) attackTimer -= deltaTime;
    if (invulnTimer > 0.f) invulnTimer -= deltaTime;

    float hpPercent = health / maxHealth;
    float aggressionMapped = (1.0f - hpPercent) / 0.8f;
    if (aggressionMapped > 1.0f) aggressionMapped = 1.0f;
    if (aggressionMapped < 0.f) aggressionMapped = 0.f;
    aggressionMultiplier = 1.0f + (aggressionMapped * 0.6f);

    shape.setRotation(sf::degrees(0.f));
    sf::Vector2f currentPos = shape.getPosition();

    for (auto it = floorFlames.begin(); it != floorFlames.end(); ) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.f) it = floorFlames.erase(it);
        else ++it;
    }

    if (currentPhase == 0) {
        isVulnerable = true;

        float idealX = playerPos.x;
        float lerpX = currentPos.x + (idealX - currentPos.x) * 1.5f * deltaTime;
        float hoverY = 150.f + std::sin(stateTimer * 2.0f) * 30.f;
        shape.setPosition(sf::Vector2f(lerpX, hoverY));

        if (attackState != 2) {
            if (std::fmod(stateTimer, 0.1f) < deltaTime) {
                if (rand() % 100 < 75) {
                    float dropX1 = 200.f + static_cast<float>(rand() % 600);
                    spawnAttack(sf::Vector2f(dropX1, -50.f), sf::Vector2f(0.f, 600.f), sf::Vector2f(70.f, 70.f), 3.f, sf::Color::White, &texFireDrop, 0);
                    float dropX2 = width - 800.f + static_cast<float>(rand() % 600);
                    spawnAttack(sf::Vector2f(dropX2, -50.f), sf::Vector2f(0.f, 600.f), sf::Vector2f(70.f, 70.f), 3.f, sf::Color::White, &texFireDrop, 0);
                }
            }
        }

        if (attackTimer <= 0.f) {
            attackTimer = 5.0f / aggressionMultiplier;
            attackState = 1 + chooseAttack(0, 4);
            subStateTimer = 0.f;
            telegraphs.clear();
        }

        if (attackState == 1) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer > 0.5f && subStateTimer < 2.5f) {
                if (std::fmod(subStateTimer, 0.15f) < deltaTime * aggressionMultiplier) {
                    float rx = 100.f + static_cast<float>(rand() % static_cast<int>(width - 200.f));
                    int type = (rand() % 10 < 3) ? 1 : 0;
                    spawnAttack(sf::Vector2f(rx, -100.f), sf::Vector2f(0.f, 1400.f), sf::Vector2f(90.f, 90.f), 3.f, sf::Color::White, type == 1 ? &texFireMeteor : &texFireDrop, type);
                }
            }
            else if (subStateTimer >= 3.0f) attackState = 0;
        }
        else if (attackState == 2) {
            subStateTimer += deltaTime * aggressionMultiplier;

            if (telegraphs.empty()) {
                sf::RectangleShape t1(sf::Vector2f(300.f, height)); t1.setPosition(sf::Vector2f(0.f, 0.f));
                sf::RectangleShape t2(sf::Vector2f(width - 300.f, height)); t2.setPosition(sf::Vector2f(800.f, 0.f));
                sf::RectangleShape t3(sf::Vector2f(300.f, height)); t3.setPosition(sf::Vector2f(width - 300.f, 0.f));
                t1.setFillColor(sf::Color(255, 0, 0, 100)); t2.setFillColor(sf::Color(255, 0, 0, 100)); t3.setFillColor(sf::Color(255, 0, 0, 100));
                telegraphs.push_back(t1); telegraphs.push_back(t2); telegraphs.push_back(t3);
            }

            float alphaPulse = 80.f + 50.f * std::sin(subStateTimer * 15.f);
            for (auto& t : telegraphs) t.setFillColor(sf::Color(255, 50, 0, static_cast<std::uint8_t>(alphaPulse)));

            if (subStateTimer >= 2.0f) {
                spawnAttack(sf::Vector2f(100.f, -150.f), sf::Vector2f(0.f, 1600.f), sf::Vector2f(300.f, 300.f), 2.5f, sf::Color::White, &texFireMeteor, 0);
                spawnAttack(sf::Vector2f(width / 2.f, -150.f), sf::Vector2f(0.f, 1600.f), sf::Vector2f(300.f, 300.f), 2.5f, sf::Color::White, &texFireMeteor, 0);
                spawnAttack(sf::Vector2f(width - 100.f, -150.f), sf::Vector2f(0.f, 1600.f), sf::Vector2f(300.f, 300.f), 2.5f, sf::Color::White, &texFireMeteor, 0);
                telegraphs.clear();
                attackState = 0;
            }
        }
        else if (attackState == 3) {
            subStateTimer += deltaTime * aggressionMultiplier;

            if (telegraphs.empty()) {
                sf::RectangleShape t1(sf::Vector2f(200.f, height));
                t1.setPosition(sf::Vector2f(width / 2.f - 100.f, 0.f));
                t1.setFillColor(sf::Color(255, 0, 0, 100));
                telegraphs.push_back(t1);
            }

            float alphaPulse = 80.f + 50.f * std::sin(subStateTimer * 15.f);
            for (auto& t : telegraphs) t.setFillColor(sf::Color(255, 50, 0, static_cast<std::uint8_t>(alphaPulse)));

            if (subStateTimer >= 1.0f && subStateTimer < 1.1f) {
                spawnAttack(sf::Vector2f(width / 2.f, height - 100.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(200.f, 200.f), 0.5f, sf::Color::White, &texFireMeteor, 0);
                subStateTimer = 1.2f;
            }
            else if (subStateTimer >= 1.5f) {
                spawnAttack(sf::Vector2f(width / 2.f - 100.f, height - 150.f), sf::Vector2f(-1400.f, 0.f), sf::Vector2f(70.f, 70.f), 4.f, sf::Color::White, &texFireDrop, 0);
                spawnAttack(sf::Vector2f(width / 2.f + 100.f, height - 150.f), sf::Vector2f(1400.f, 0.f), sf::Vector2f(70.f, 70.f), 4.f, sf::Color::White, &texFireDrop, 0);
                telegraphs.clear();
                attackState = 0;
            }
        }
        else if (attackState == 4) {
            subStateTimer += deltaTime * aggressionMultiplier;

            if (telegraphs.empty()) {
                sf::RectangleShape t1(sf::Vector2f(150.f, height));
                t1.setPosition(sf::Vector2f(width / 2.f - 75.f, 0.f));
                t1.setFillColor(sf::Color(255, 0, 0, 100));
                telegraphs.push_back(t1);
            }

            float alphaPulse = 80.f + 50.f * std::sin(subStateTimer * 15.f);
            for (auto& t : telegraphs) t.setFillColor(sf::Color(255, 50, 0, static_cast<std::uint8_t>(alphaPulse)));

            if (subStateTimer >= 1.8f) {
                spawnAttack(sf::Vector2f(width / 2.f, height / 2.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(150.f, height), 2.5f, sf::Color::White, &texFireFloor, 1);
                telegraphs.clear();
                attackState = 0;
            }
        }
    }
    else if (currentPhase == 1) {
        if (attackState == 0) {
            isVulnerable = false;
            float idealSwimX = width / 2.f + std::sin(stateTimer * 0.5f * aggressionMultiplier) * (width / 2.f - 200.f);
            float lerpX = currentPos.x + (idealSwimX - currentPos.x) * 4.0f * deltaTime;
            shape.setPosition(sf::Vector2f(lerpX, height - 100.f));

            if (attackTimer <= 0.f) {
                attackTimer = 6.0f / aggressionMultiplier;
                attackState = 1 + chooseAttack(1, 4);
                subStateTimer = 0.f;
                telegraphs.clear();

                if (attackState == 1) {
                    savedTargetPos.x = (currentPos.x < width / 3.f) ? 950.f : ((currentPos.x > 2.f * width / 3.f) ? (width - 950.f) : (width / 2.f));
                    savedTargetPos.y = height - 150.f;
                }
                else if (attackState == 2) {
                    savedTargetPos.x = (rand() % 2 == 0) ? 200.f : width - 200.f;
                    savedTargetPos.y = height - 200.f;
                }
                else if (attackState == 3 || attackState == 4) {
                    savedTargetPos.x = width / 2.f;
                    savedTargetPos.y = height - 100.f;
                }
            }
        }
        else if (attackState == 1) {
            isVulnerable = false;
            subStateTimer += deltaTime * aggressionMultiplier;

            if (subStateTimer <= 1.5f) {
                float lerpX = currentPos.x + (savedTargetPos.x - currentPos.x) * 4.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, savedTargetPos.y));
            }
            else if (subStateTimer <= 3.0f) {
                shape.setRotation(sf::degrees(subStateTimer * 3000.f));
                shape.setPosition(sf::Vector2f(savedTargetPos.x, savedTargetPos.y));
            }
            else if (subStateTimer >= 3.0f && subStateTimer < 3.1f) {
                if (savedTargetPos.x != 950.f) spawnAttack(sf::Vector2f(950.f, height - 400.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(160.f, 800.f), 1.5f, sf::Color::White, &texWaterGeyser, 1);
                if (savedTargetPos.x != width / 2.f) spawnAttack(sf::Vector2f(width / 2.f, height - 400.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(160.f, 800.f), 1.5f, sf::Color::White, &texWaterGeyser, 1);
                if (savedTargetPos.x != width - 950.f) spawnAttack(sf::Vector2f(width - 950.f, height - 400.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(160.f, 800.f), 1.5f, sf::Color::White, &texWaterGeyser, 1);
                subStateTimer = 3.2f;
            }
            else if (subStateTimer >= 4.5f) attackState = 0;
        }
        else if (attackState == 2) {
            isVulnerable = true;
            subStateTimer += deltaTime * aggressionMultiplier;

            if (subStateTimer <= 1.5f) {
                float lerpX = currentPos.x + (savedTargetPos.x - currentPos.x) * 4.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, savedTargetPos.y));
            }
            else if (subStateTimer <= 3.5f) {
                float t = (subStateTimer - 1.5f) / 2.0f;
                float targetJumpX = (savedTargetPos.x < width / 2.f) ? (width - 200.f) : 200.f;

                float jx = savedTargetPos.x + (targetJumpX - savedTargetPos.x) * t;
                float jy = savedTargetPos.y - std::sin(t * 3.1415f) * 800.f;
                shape.setPosition(sf::Vector2f(jx, jy));

                if (std::abs(t - 0.5f) < 0.01f) {
                    spawnAttack(sf::Vector2f(jx, jy), sf::Vector2f(-500.f, 400.f), sf::Vector2f(35.f, 35.f), 3.f, sf::Color::White, &texWaterBubble, 0);
                    spawnAttack(sf::Vector2f(jx, jy), sf::Vector2f(0.f, 700.f), sf::Vector2f(35.f, 35.f), 3.f, sf::Color::White, &texWaterBubble, 0);
                    spawnAttack(sf::Vector2f(jx, jy), sf::Vector2f(500.f, 400.f), sf::Vector2f(35.f, 35.f), 3.f, sf::Color::White, &texWaterBubble, 0);
                }
            }
            else attackState = 0;
        }
        else if (attackState == 3) {
            isVulnerable = true;
            subStateTimer += deltaTime * aggressionMultiplier;

            if (subStateTimer <= 1.0f) {
                float lerpX = currentPos.x + (savedTargetPos.x - currentPos.x) * 4.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, savedTargetPos.y));
            }
            else if (subStateTimer <= 3.5f) {
                shape.setRotation(sf::degrees(subStateTimer * 4000.f));

                if (std::fmod(subStateTimer, 0.45f) < deltaTime * aggressionMultiplier) {
                    sf::Vector2f dir = playerPos - shape.getPosition();
                    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (dist != 0) { dir.x /= dist; dir.y /= dist; }
                    spawnAttack(shape.getPosition(), sf::Vector2f(dir.x * 450.f, dir.y * 450.f), sf::Vector2f(30.f, 30.f), 5.f, sf::Color::White, &texWaterBubble, 0);
                }
            }
            else attackState = 0;
        }
        else if (attackState == 4) {
            isVulnerable = true;
            subStateTimer += deltaTime * aggressionMultiplier;

            if (subStateTimer <= 1.0f) {
                float lerpX = currentPos.x + (width / 2.f - currentPos.x) * 4.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, height - 100.f));
            }
            else if (subStateTimer >= 2.0f && subStateTimer < 2.1f) {
                float targetsX[3] = { 950.f, width / 2.f, width - 950.f };
                for (int i = 0; i < 3; ++i) {
                    spawnAttack(sf::Vector2f(targetsX[i], height - 100.f), sf::Vector2f(0.f, -1800.f), sf::Vector2f(60.f, 200.f), 6.f, sf::Color::White, &texWaterKnife, 4);
                }
                subStateTimer = 2.2f;
            }
            else if (subStateTimer >= 3.5f) attackState = 0;
        }
    }
    else if (currentPhase == 2) {
        isVulnerable = true;
        shape.setPosition(earthWall.getPosition() + sf::Vector2f(200.f, height / 2.f));

        if (attackState == 0) {
            if (std::fmod(stateTimer, 1.0f / aggressionMultiplier) < deltaTime) {
                sf::Vector2f dir = playerPos - shape.getPosition();
                float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (dist != 0) { dir.x /= dist; dir.y /= dist; }
                spawnAttack(shape.getPosition(), sf::Vector2f(dir.x * 1000.f, dir.y * 1000.f), sf::Vector2f(45.f, 45.f), 5.f, sf::Color::White, &texEarthShot, 2);
            }
        }

        if (attackTimer <= 0.f) {
            attackTimer = 2.5f / aggressionMultiplier;
            attackState = 1 + chooseAttack(2, 4);
            subStateTimer = 0.f;
            telegraphs.clear();
        }

        if (attackState == 1) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer > 0.5f && subStateTimer < 0.55f) {
                spawnAttack(sf::Vector2f(width, height - 75.f), sf::Vector2f(-1200.f, 0.f), sf::Vector2f(80.f, 100.f), 6.f, sf::Color::White, &texBossGround, 2);
                subStateTimer = 0.6f;
            }
            else if (subStateTimer > 1.2f && subStateTimer < 1.25f) {
                spawnAttack(sf::Vector2f(width, height - 125.f), sf::Vector2f(-1200.f, 0.f), sf::Vector2f(80.f, 200.f), 6.f, sf::Color::White, &texBossGround, 2);
                subStateTimer = 1.3f;
            }
            else if (subStateTimer > 1.9f && subStateTimer < 1.95f) {
                spawnAttack(sf::Vector2f(width, height - 200.f), sf::Vector2f(-1200.f, 0.f), sf::Vector2f(80.f, 300.f), 6.f, sf::Color::White, &texBossGround, 2);
                attackState = 0;
            }
        }
        else if (attackState == 2) {
            subStateTimer += deltaTime * aggressionMultiplier;
            float cycle = std::fmod(subStateTimer, 2.5f);

            if (cycle <= 0.8f) {
                sf::Vector2f dir = playerPos - shape.getPosition();
                float angle = std::atan2(dir.y, dir.x);

                if (telegraphs.empty()) {
                    sf::RectangleShape laser(sf::Vector2f(3500.f, 5.f));
                    laser.setOrigin(sf::Vector2f(0.f, 2.5f));
                    laser.setFillColor(sf::Color(255, 0, 0, 100));
                    telegraphs.push_back(laser);
                }
                for (auto& t : telegraphs) {
                    t.setPosition(shape.getPosition());
                    t.setRotation(sf::degrees(angle * 180.f / 3.1415f));
                }
            }
            else if (cycle > 0.8f && cycle <= 1.4f) {
                for (auto& t : telegraphs) t.setFillColor(sf::Color(255, 0, 0, 220));
            }
            else if (cycle > 1.4f && cycle < 1.5f) {
                if (!telegraphs.empty()) {
                    float angle = telegraphs[0].getRotation().asDegrees();
                    float radAngle = angle * 3.1415f / 180.f;
                    sf::Vector2f fireDir(std::cos(radAngle), std::sin(radAngle));

                    spawnAttack(shape.getPosition(), sf::Vector2f(0.f, 0.f), sf::Vector2f(3000.f, 80.f), 1.0f, sf::Color::White, &texEarthLaser, 1);
                    attacks.back().shape.setOrigin(sf::Vector2f(0.f, 40.f));
                    attacks.back().shape.setRotation(sf::degrees(angle));
                    telegraphs.clear();
                }
            }

            if (subStateTimer >= 3.6f) {
                attackState = 0;
                telegraphs.clear();
            }
        }
        else if (attackState == 3) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer <= 3.0f) {
                if (std::fmod(subStateTimer, 0.1f) < deltaTime * aggressionMultiplier) {
                    float rx = static_cast<float>(rand() % static_cast<int>(width));
                    spawnAttack(sf::Vector2f(rx, -50.f), sf::Vector2f(0.f, 1800.f), sf::Vector2f(120.f, 120.f), 3.f, sf::Color::White, &texEarthRock, 2);
                }
            }
            else attackState = 0;
        }
        else if (attackState == 4) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer > 0.5f && subStateTimer < 3.0f) {
                if (std::fmod(subStateTimer, 0.6f) < deltaTime * aggressionMultiplier) {
                    sf::Vector2f dir = playerPos - shape.getPosition();
                    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (dist != 0) { dir.x /= dist; dir.y /= dist; }
                    spawnAttack(shape.getPosition(), sf::Vector2f(dir.x * 1500.f, dir.y * 1500.f), sf::Vector2f(70.f, 70.f), 4.f, sf::Color::White, &texEarthShot, 2);
                }
            }
            else if (subStateTimer >= 3.0f) attackState = 0;
        }
    }
    else if (currentPhase == 3) {
        float cx = width / 2.f;
        float cy = height / 2.f + 100.f;
        float offsetY = 350.f;

        if (attackState == 0) {
            float ox = cx + std::cos(stateTimer * 1.5f * aggressionMultiplier) * (width / 2.f - 300.f);
            float oy = cy - 500.f + std::sin(stateTimer * 3.0f * aggressionMultiplier) * 150.f;
            shape.setPosition(sf::Vector2f(ox, oy));

            if (std::fmod(stateTimer, 0.1f / aggressionMultiplier) < deltaTime) {
                float angle = stateTimer * 8.0f;
                sf::Vector2f dir(std::cos(angle), std::sin(angle));
                spawnAttack(shape.getPosition(), sf::Vector2f(dir.x * 600.f, dir.y * 600.f), sf::Vector2f(30.f, 30.f), 4.f, sf::Color::White, &texAirShot, 0);
            }

            if (attackTimer <= 0.f) {
                attackTimer = 5.0f / aggressionMultiplier;
                attackState = 1 + chooseAttack(3, 4);
                subStateTimer = 0.f;
                telegraphs.clear();

                float levels[3] = { cy - offsetY, cy, cy + offsetY };
                float chosenY = levels[rand() % 3];
                dashStartPos = (rand() % 2 == 0) ? sf::Vector2f(200.f, chosenY) : sf::Vector2f(width - 200.f, chosenY);

                if (attackState == 2) dashStartPos = sf::Vector2f(cx, 200.f);
            }
        }
        else if (attackState == 1 || attackState == 2) {
            subStateTimer += deltaTime * aggressionMultiplier;

            if (subStateTimer <= 1.0f) {
                float lerpX = currentPos.x + (dashStartPos.x - currentPos.x) * 5.0f * deltaTime;
                float lerpY = currentPos.y + (dashStartPos.y - currentPos.y) * 5.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, lerpY));
            }
            else if (subStateTimer <= 1.8f) {
                shape.setPosition(sf::Vector2f(dashStartPos.x + (rand() % 10 - 5), dashStartPos.y + (rand() % 10 - 5)));
                if (telegraphs.empty()) {
                    if (attackState == 1) {
                        sf::RectangleShape t(sf::Vector2f(width, 5.f));
                        t.setPosition(sf::Vector2f(0.f, dashStartPos.y - 2.5f));
                        t.setFillColor(sf::Color(0, 255, 0, 180)); telegraphs.push_back(t);
                    }
                    else {
                        sf::RectangleShape t(sf::Vector2f(5.f, height));
                        t.setPosition(sf::Vector2f(dashStartPos.x - 2.5f, 0.f));
                        t.setFillColor(sf::Color(0, 255, 0, 180)); telegraphs.push_back(t);
                    }
                }
            }
            else if (subStateTimer <= 2.2f) {
                float t = (subStateTimer - 1.8f) / 0.4f;
                if (attackState == 1) {
                    float endX = (dashStartPos.x < cx) ? (width - 200.f) : 200.f;
                    shape.setPosition(sf::Vector2f(dashStartPos.x + (endX - dashStartPos.x) * t, dashStartPos.y));
                }
                else {
                    float endY = height - 100.f;
                    shape.setPosition(sf::Vector2f(dashStartPos.x, dashStartPos.y + (endY - dashStartPos.y) * t));
                }
            }
            else { attackState = 0; telegraphs.clear(); }
        }
        else if (attackState == 3) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer <= 1.0f) {
                float lerpX = currentPos.x + (width / 2.f - currentPos.x) * 5.0f * deltaTime;
                float lerpY = currentPos.y + (200.f - currentPos.y) * 5.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, lerpY));
            }
            else if (subStateTimer <= 3.0f) {
                if (std::fmod(subStateTimer, 0.25f) < deltaTime * aggressionMultiplier) {
                    spawnAttack(sf::Vector2f(playerPos.x, 0.f), sf::Vector2f(0.f, 2000.f), sf::Vector2f(60.f, 150.f), 4.f, sf::Color::White, &texAirDrop, 1);
                }
            }
            else if (subStateTimer >= 4.0f) attackState = 0;
        }
        else if (attackState == 4) {
            subStateTimer += deltaTime * aggressionMultiplier;
            if (subStateTimer <= 1.0f) {
                float targetY = (playerPos.y < height / 2.f) ? 300.f : height - 300.f;
                float lerpX = currentPos.x + (200.f - currentPos.x) * 5.0f * deltaTime;
                float lerpY = currentPos.y + (targetY - currentPos.y) * 5.0f * deltaTime;
                shape.setPosition(sf::Vector2f(lerpX, lerpY));
            }
            else if (subStateTimer <= 3.0f) {
                if (std::fmod(subStateTimer, 0.4f) < deltaTime * aggressionMultiplier) {
                    spawnAttack(shape.getPosition(), sf::Vector2f(2200.f, 0.f), sf::Vector2f(150.f, 50.f), 4.f, sf::Color::White, &texAirSlash, 1);
                }
            }
            else if (subStateTimer >= 3.5f) attackState = 0;
        }
    }

    for (auto it = attacks.begin(); it != attacks.end(); ) {
        if (it->type == 4) {
            it->velocity.y += 2400.f * deltaTime;
            float angle = std::atan2(it->velocity.y, it->velocity.x) * 180.f / 3.14159f;
            it->shape.setRotation(sf::degrees(angle));
        }

        it->shape.move(it->velocity * deltaTime);
        it->lifetime -= deltaTime;

        if (currentPhase == 1 && it->type == 0) it->shape.rotate(sf::degrees(800.f * deltaTime));
        else if (currentPhase == 3 && it->type == 0) it->shape.rotate(sf::degrees(2500.f * deltaTime));

        if (currentPhase == 0 && it->type == 1 && (it->lifetime <= 0.f || it->shape.getPosition().y >= height - 50.f)) {
            if (floorFlames.size() < 10) {
                BossAttack flame;
                flame.shape.setSize(sf::Vector2f(200.f, 80.f));
                flame.shape.setOrigin(sf::Vector2f(100.f, 80.f));
                flame.shape.setPosition(sf::Vector2f(it->shape.getPosition().x, height - 40.f));
                flame.shape.setTexture(&texFireFloor);
                flame.shape.setFillColor(sf::Color(255, 255, 255, 220));
                flame.active = true;
                flame.lifetime = 5.0f;
                floorFlames.push_back(flame);
            }
            it = attacks.erase(it);
            continue;
        }

        if (it->lifetime <= 0.f || !it->active) it = attacks.erase(it);
        else ++it;
    }

    bossSprite.setPosition(shape.getPosition());

    float baseScale = 1.0f;
    if (currentPhase == 0) baseScale = 6.0f;
    else if (currentPhase == 1) baseScale = 6.0f;
    else if (currentPhase == 3) baseScale = 6.0f;

    float currentScaleX = baseScale;
    float currentScaleY = baseScale;

    if (currentPhase == 0 || currentPhase == 3) {
        if (playerPos.x > shape.getPosition().x) currentScaleX = -baseScale;
    }

    bossSprite.setScale(sf::Vector2f(currentScaleX, currentScaleY));
}

void Boss::draw(sf::RenderWindow& window) {
    if (currentPhase == 1) window.draw(waterFloor);
    if (currentPhase == 2) window.draw(earthWall);

    for (const auto& t : telegraphs) window.draw(t);
    for (const auto& plat : platforms) window.draw(plat);
    for (const auto& atk : attacks) window.draw(atk.shape);
    for (const auto& flame : floorFlames) window.draw(flame.shape);

    if (health > 0.f && currentPhase != 2 && (isVulnerable || currentPhase == 1)) {
        window.draw(bossSprite);
    }

    if (health > 0.f) {
        window.draw(hpBarBg);
        window.draw(hpBarFg);
    }
}

void Boss::spawnAttack(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f size, float life, sf::Color color, const sf::Texture* tex, int type) {
    BossAttack atk;
    atk.shape.setSize(size);
    atk.shape.setOrigin(sf::Vector2f(size.x / 2.f, size.y / 2.f));
    atk.shape.setPosition(pos);

    if (tex) {
        atk.shape.setTexture(tex);
        atk.shape.setFillColor(sf::Color(255, 255, 255, color.a));

        if (tex == &texBossGround) {
            atk.shape.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(size.x), static_cast<int>(size.y) }));
        }
    }
    else {
        atk.shape.setFillColor(color);
    }

    atk.velocity = vel;
    atk.lifetime = life;
    atk.active = true;
    atk.type = type;
    attacks.push_back(atk);
}

std::vector<sf::FloatRect> Boss::getAttackBounds() const {
    std::vector<sf::FloatRect> bounds;
    for (const auto& atk : attacks) bounds.push_back(atk.shape.getGlobalBounds());
    for (const auto& flame : floorFlames) bounds.push_back(flame.shape.getGlobalBounds());
    if (currentPhase == 1) bounds.push_back(waterFloor.getGlobalBounds());
    if (currentPhase == 3 && attackState > 0 && subStateTimer > 1.8f) bounds.push_back(shape.getGlobalBounds());
    return bounds;
}

std::vector<BossAttack>& Boss::getAttacks() {
    return attacks;
}

bool Boss::checkEarthCrush() {
    return false;
}

void Boss::takeDamage(int dmg) {
    if (invulnTimer <= 0.f && isVulnerable) {
        health -= dmg;
        invulnTimer = 0.1f;
        if (health <= phaseThreshold) {
            phaseThreshold -= maxHealth * 0.10f;
            nextPhase();
        }
    }
}

float Boss::getX() const { return shape.getPosition().x; }
float Boss::getY() const { return shape.getPosition().y; }
int Boss::getCurrentPhase() const { return currentPhase; }
bool Boss::isAlive() const { return health > 0.f; }
sf::FloatRect Boss::getBounds() const { return shape.getGlobalBounds(); }

void Boss::nextPhase() {
    int oldPhase = currentPhase;
    do {
        currentPhase = rand() % 4;
    } while (currentPhase == oldPhase);

    stateTimer = 0.f;
    attackTimer = 0.f;
    attackState = 0;
    subStateTimer = 0.f;
    telegraphs.clear();
    attacks.clear();
    floorFlames.clear();
    setupPhase();
}

std::vector<sf::FloatRect> Boss::getPlatformBounds() const {
    std::vector<sf::FloatRect> bounds;
    for (const auto& plat : platforms) bounds.push_back(plat.getGlobalBounds());
    return bounds;
}

void Boss::reset(float screenWidth, float screenHeight) {
    currentPhase = 0;
    attackTimer = 0.f;
    attackState = 0;
    subStateTimer = 0.f;
    stateTimer = 0.f;

    attacks.clear();
    floorFlames.clear();
    telegraphs.clear();

    health = maxHealth;
    phaseThreshold = maxHealth * 0.90f;

    isVulnerable = true;
    invulnTimer = 0.f;

    setupPhase();
}