#include "player.h"
#include "../area/area.h"
#include <cmath>
#include <cstdlib>
#include <cstdint>

Player::Player(float startX, float startY) : movementHandler(400.f, 1000.f, 1200.f, 800.f), headSprite(texProjWater) {
    shape.setSize(sf::Vector2f(40.f, 80.f));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(0.f);
    shape.setPosition(sf::Vector2f(startX, startY));

    health = 10;
    elementCycle = { 1, 2, 0, 3 };
    currentElementIndex = 0;
    currentElementType = elementCycle[currentElementIndex];
    chargeTimer = 0.f;
    isFullyCharged = false;
    prevZPressed = false;
    prevXPressed = false;
    prevCPressed = false;
    totalTime = 0.f;
    airCooldownTimer = 0.f;

    headAnimTimer = 0.f;
    headSeqIndex = 0;
    hoverOffset = 0.f;

    cloak.initialize(32, 90.f, 170.f);

    texProjFire.loadFromFile("assets/player_projectile_fire.png");
    texProjWater.loadFromFile("assets/player_projectile_water.png");
    texProjEarth.loadFromFile("assets/player_projectile_earth.png");
    texProjAir.loadFromFile("assets/player_projectile_air.png");

    for (int i = 0; i < 5; ++i) sounds.emplace_back(soundBuffers[i]);
}

void Player::playSound(int index) {
    if (index >= 0 && index < 5) sounds[index].play();
}

void Player::update(float deltaTime, float screenWidth, float screenHeight, const std::vector<sf::FloatRect>& platforms) {
    totalTime += deltaTime;
    if (airCooldownTimer > 0.f) airCooldownTimer -= deltaTime;

    movementHandler.handleInput(deltaTime);
    movementHandler.applyPhysics(deltaTime);
    movementHandler.updatePosition(shape, deltaTime);

    sf::Vector2f currentVelocity = movementHandler.getVelocity();
    sf::FloatRect playerBounds = shape.getGlobalBounds();
    bool currentlyGrounded = false;

    if (currentVelocity.y >= 0.f) {
        for (const auto& plat : platforms) {
            auto intersection = playerBounds.findIntersection(plat);
            if (intersection.has_value()) {
                if (playerBounds.position.y + playerBounds.size.y - intersection->size.y <= plat.position.y + 10.f) {
                    shape.setPosition(sf::Vector2f(shape.getPosition().x, plat.position.y - playerBounds.size.y));
                    currentVelocity.y = 0.f;
                    currentlyGrounded = true;
                    playerBounds = shape.getGlobalBounds();
                }
            }
        }
    }

    bool hitFloor = Area::enforceBoundaries(shape, currentVelocity, screenWidth, screenHeight);
    if (hitFloor) currentlyGrounded = true;

    movementHandler.setGrounded(currentlyGrounded);
    movementHandler.setVelocity(currentVelocity);

    sf::Vector2f playerPos = shape.getPosition();

    float cloakX = playerPos.x + 30.f;
    float cloakY = playerPos.y - 20.f;
    cloak.setPosition(sf::Vector2f(cloakX, cloakY));
    cloak.update(deltaTime, currentVelocity);

    float facing = movementHandler.getFacingDirection();
    float aimX = 0.f, aimY = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) aimX -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) aimX += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) aimY -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) aimY += 1.f;
    if (aimX == 0.f && aimY == 0.f) {
        aimX = facing;
        aimY = 0.f;
    }

    bool xPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
    bool cPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);

    if (!isFullyCharged) {
        if (xPressed && !prevXPressed) {
            currentElementIndex = (currentElementIndex - 1 + 4) % 4;
            chargeTimer = 0.f; headSeqIndex = 0;
        }
        if (cPressed && !prevCPressed) {
            currentElementIndex = (currentElementIndex + 1) % 4;
            chargeTimer = 0.f; headSeqIndex = 0;
        }
    }
    prevXPressed = xPressed;
    prevCPressed = cPressed;
    currentElementType = elementCycle[currentElementIndex];

    int hFrameWidth = 50, hFrameHeight = 50;
    float hFrameTime = 0.1f;
    int hLoopStart = 0;
    std::vector<int> hSeq;

    if (currentElementType == 0) {
        hFrameWidth = 52; hFrameHeight = 48; hFrameTime = 0.08f;
        hSeq = { 0, 1, 2, 3, 4, 5, 4, 3 }; hLoopStart = 2;
        headSprite.setTexture(texProjWater);
    }
    else if (currentElementType == 1) {
        hFrameWidth = 52; hFrameHeight = 29; hFrameTime = 0.1f;
        hSeq = { 0, 1, 2 }; hLoopStart = 0;
        headSprite.setTexture(texProjFire);
    }
    else if (currentElementType == 2) {
        hFrameWidth = 65; hFrameHeight = 40; hFrameTime = 0.08f;
        hSeq = { 0, 1, 2, 3, 4 }; hLoopStart = 1;
        headSprite.setTexture(texProjAir);
    }
    else if (currentElementType == 3) {
        hFrameWidth = texProjEarth.getSize().x; hFrameHeight = texProjEarth.getSize().y; hFrameTime = 1.0f;
        hSeq = { 0 }; hLoopStart = 0;
        headSprite.setTexture(texProjEarth);
    }

    headAnimTimer += deltaTime;
    if (headAnimTimer >= hFrameTime) {
        headAnimTimer -= hFrameTime;
        headSeqIndex++;
        if (headSeqIndex >= hSeq.size()) headSeqIndex = hLoopStart;
    }
    if (headSeqIndex >= hSeq.size()) headSeqIndex = 0;

    int currentFrame = hSeq[headSeqIndex];
    headSprite.setTextureRect(sf::IntRect({ currentFrame * hFrameWidth, 0 }, { hFrameWidth, hFrameHeight }));
    headSprite.setOrigin(sf::Vector2f(static_cast<float>(hFrameWidth) / 2.f, static_cast<float>(hFrameHeight) / 2.f));

    float headScaleBase = 75.f / static_cast<float>(hFrameWidth);
    float headScaleX = headScaleBase;
    headSprite.setScale(sf::Vector2f(headScaleX, headScaleBase));


    hoverOffset = std::sin(totalTime * 4.0f) * 6.0f;
    headSprite.setPosition(sf::Vector2f(playerPos.x + 75.f, playerPos.y - 55.f + hoverOffset));

    if (isFullyCharged || (currentElementType == 0 && chargeTimer >= 1.0f)) {
        float flashCycle = std::fmod(totalTime, 0.2f);
        if (flashCycle < 0.1f) headSprite.setColor(sf::Color(255, 255, 255, 128));
        else headSprite.setColor(sf::Color::White);
    }
    else {
        headSprite.setColor(sf::Color::White);
    }

    bool zPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z);
    float spawnX = playerPos.x + 30.f;
    float spawnY = playerPos.y + 60.f;

    float maxCharge = 5.0f;
    if (currentElementType == 3) maxCharge = 1.0f;
    if (currentElementType == 2) maxCharge = 2.0f;

    if (currentElementType == 1 || currentElementType == 3) {
		if (currentElementType == 1){
            headSprite.setRotation(sf::degrees(90.f));
        }
        else
        {
            headSprite.setRotation(sf::degrees(0.f));
        }
        if (zPressed && prevZPressed && !isFullyCharged && projectiles.empty()) {
            chargeTimer += deltaTime;
            if (chargeTimer >= maxCharge) { chargeTimer = maxCharge; isFullyCharged = true; }
        }
        else if (zPressed && !prevZPressed && isFullyCharged) {
            const sf::Texture* tex = (currentElementType == 1) ? &texProjFire : &texProjEarth;
            projectiles.emplace_back(spawnX, spawnY, aimX, aimY, currentElementType, chargeTimer, tex);
            isFullyCharged = false; chargeTimer = 0.f; cloak.setCurrentColor(sf::Color(128, 128, 128));
        }
        else if (!zPressed && prevZPressed && !isFullyCharged) chargeTimer = 0.f;
    }
    else if (currentElementType == 0) {
        headSprite.setRotation(sf::degrees(90.f));
        if (zPressed && projectiles.empty()) {
            chargeTimer += deltaTime;
            if (chargeTimer > 5.0f) chargeTimer = 5.0f;
        }
        else if (!zPressed && prevZPressed) {
            if (chargeTimer >= 1.0f) {
                projectiles.emplace_back(spawnX, spawnY, aimX, aimY, 0, chargeTimer, &texProjWater);
            }
            chargeTimer = 0.f; cloak.setCurrentColor(sf::Color(128, 128, 128));
        }
    }
    else if (currentElementType == 2) {
        headSprite.setRotation(sf::degrees(90.f));
        if (zPressed) {
            chargeTimer += deltaTime;
            if (chargeTimer > maxCharge) chargeTimer = maxCharge;
            if (airCooldownTimer <= 0.15f) {
                projectiles.emplace_back(spawnX, spawnY, aimX, aimY, 2, 0.f, &texProjAir);
                airCooldownTimer = 0.15f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.35f));
            }
        }
        else chargeTimer = 0.f;
    }
    prevZPressed = zPressed;

    sf::Color gray(128, 128, 128);
    sf::Color elementColors[4] = { sf::Color::Blue, sf::Color::Red, sf::Color::Green, sf::Color(139, 69, 19) };
    sf::Color targetColor = elementColors[currentElementType];

    if (!isFullyCharged && chargeTimer > 0.f) {
        float blendRatio = chargeTimer / maxCharge;
        sf::Color cloakColor(
            static_cast<std::uint8_t>(gray.r + (targetColor.r - gray.r) * blendRatio),
            static_cast<std::uint8_t>(gray.g + (targetColor.g - gray.g) * blendRatio),
            static_cast<std::uint8_t>(gray.b + (targetColor.b - gray.b) * blendRatio)
        );
        cloak.setCurrentColor(cloakColor);
    }
    else if (isFullyCharged || (currentElementType == 0 && chargeTimer >= 1.0f)) cloak.setCurrentColor(targetColor);
    else cloak.setCurrentColor(gray);

    for (auto it = projectiles.begin(); it != projectiles.end();) {
        it->update(deltaTime, sf::Vector2f(spawnX, spawnY), aimX, aimY);
        if (!it->isActive()) it = projectiles.erase(it);
        else ++it;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(cloak);

    if (projectiles.empty() || chargeTimer > 0.f || isFullyCharged) {
        window.draw(headSprite);
    }

    for (auto& proj : projectiles) {
        proj.draw(window);
    }
}

int Player::getHealth() const { return health; }
float Player::getMaxHealth() const { return health < 10 ? 10.0f : 10.0f; }
void Player::setHealth(int newHealth) { health = newHealth; }
std::vector<PlayerProjectile>& Player::getProjectiles() { return projectiles; }
sf::Vector2f Player::getPosition() const { return shape.getPosition(); }

void Player::resetPositionForPhase(int phase, float screenWidth, float screenHeight) {
    movementHandler.setVelocity(sf::Vector2f(0.f, 0.f));
    if (phase == 0) shape.setPosition(sf::Vector2f(500.f, 700.f - 200.f));
    else if (phase == 1) shape.setPosition(sf::Vector2f((screenWidth / 2.f) - 50.f, 1100.f - 200.f));
    else if (phase == 2) shape.setPosition(sf::Vector2f(200.f, screenHeight - 200.f));
    else if (phase == 3) {
        float cx = screenWidth / 2.f;
        float cy = screenHeight / 2.f + 100.f;
        shape.setPosition(sf::Vector2f(cx - 50.f, (cy + 350.f) - 200.f));
    }
}

void Player::reset(float startX, float startY) {
    shape.setPosition(sf::Vector2f(startX, startY));
    health = 10;
    currentElementIndex = 0;
    currentElementType = elementCycle[currentElementIndex];
    chargeTimer = 0.f;
    isFullyCharged = false;
    projectiles.clear();
    movementHandler.setVelocity(sf::Vector2f(0.f, 0.f));
    cloak.setCurrentColor(sf::Color(128, 128, 128));
}