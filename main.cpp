#include <iostream>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "interface.h"
#include "player.h"
#include "boss.h"
#include "area.h"
#include <SFML/Audio.hpp>
#include <ctime>



enum class GameState { MENU, GAME, GAME_OVER };


int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    sf::RenderWindow window(sf::VideoMode({ 3440, 1440 }), "Dance of Colors");
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect({ 0.f, 0.f }, { 3440.f, 1440.f }));
    window.setView(view);

    GameClock gameClock;
    Interface gameInterface;

	sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("assets/BackgroundMusic.mp3")) {
        std::cerr << "failed to load background music!\n";
    }
    backgroundMusic.setLooping(true);
    backgroundMusic.setVolume(10.f);


    if (!gameInterface.loadResources()) {
        std::cerr << "failed to load interface resources!\n";
        return -1;
    }

    MainMenu mainMenu(3440.f, 1440.f);
    mainMenu.loadResources(gameInterface.getFont());

    GameOverMenu gameOverMenu(3440.f, 1440.f);
    gameOverMenu.loadResources(gameInterface.getFont());

    Area gameArea(3440.f, 1440.f);
    if (!gameArea.loadResources()) {
        std::cerr << "failed to load area resources!\n";
        return -1;
    }

    Player player(100.f, 200.f);
    Boss boss(3440.f, 1440.f);

    sf::Clock deltaClock;
    bool prevVPressed = false;
    float playerInvulnTimer = 0.f;
    int lastPhase = boss.getCurrentPhase();
    float score = 0.0f;

    GameState currentState = GameState::MENU;
    bool showHitboxes = false;
    bool prevHPressed = false;
    while (window.isOpen()) {
        float deltaTime = deltaClock.restart().asSeconds();
        float elapsedTime = gameClock.getElapsedTime();
        float currentMultiplier = 5.0f - (4.0f * (elapsedTime / 600.0f));

        if (currentMultiplier < 1.0f) {
            currentMultiplier = 1.0f;
        }

        score += 100.0f * currentMultiplier * deltaTime;

        std::optional<sf::Event> event;

        while ((event = window.pollEvent())) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (currentState == GameState::MENU) {
                if (auto const* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Up) {
                        mainMenu.moveUp();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Down) {
                        mainMenu.moveDown();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (mainMenu.getSelectedOption() == 0) {
                            player.reset(100.f, 200.f);
                            boss.reset(3440.f, 1440.f);
                            playerInvulnTimer = 0.f;
                            score = 0;
                            lastPhase = boss.getCurrentPhase();
                            player.resetPositionForPhase(lastPhase, 3440.f, 1440.f);
                            currentState = GameState::GAME;
                        }
                        else if (mainMenu.getSelectedOption() == 1) {
                            mainMenu.cycleResolution(gameInterface.getFont());
                            auto res = mainMenu.getCurrentResolution();
                            window.create(sf::VideoMode(res), "Dance of Colors");
                            window.setFramerateLimit(60);
                            window.setView(view);
                        }
                        else if (mainMenu.getSelectedOption() == 2) {
                            window.close();
                        }
                    }
                }
            }
            else if (currentState == GameState::GAME_OVER) {
                backgroundMusic.stop();
                if (auto const* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Up) {
                        gameOverMenu.moveUp();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Down) {
                        gameOverMenu.moveDown();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (gameOverMenu.getSelectedOption() == 0) {
                            player.reset(100.f, 200.f);
                            boss.reset(3440.f, 1440.f);
                            playerInvulnTimer = 0.f;
                            score = 0;
                            lastPhase = boss.getCurrentPhase();
                            player.resetPositionForPhase(lastPhase, 3440.f, 1440.f);
                            currentState = GameState::GAME;
                        }
                        else if (gameOverMenu.getSelectedOption() == 1) {
                            currentState = GameState::MENU;
                        }
                    }
                }
            }
        }

        if (currentState == GameState::GAME) {
            if (backgroundMusic.getStatus() != sf::Music::Status::Playing) {
                backgroundMusic.play();
            }
            if (playerInvulnTimer > 0.f) playerInvulnTimer -= deltaTime;

            bool hPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H);
            if (hPressed && !prevHPressed) {
                showHitboxes = !showHitboxes;
            }
            prevHPressed = hPressed;
            bool vPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V);
            if (vPressed && !prevVPressed) {
                boss.nextPhase();
            }
            prevVPressed = vPressed;

            boss.update(deltaTime, player.getPosition());
            player.update(deltaTime, 3440.f, 1440.f, boss.getPlatformBounds());
            gameArea.draw(window, boss.getCurrentPhase());

            if (boss.getCurrentPhase() != lastPhase) {
                player.resetPositionForPhase(boss.getCurrentPhase(), 3440.f, 1440.f);
                lastPhase = boss.getCurrentPhase();
            }

            if (playerInvulnTimer <= 0.f) {
                sf::FloatRect playerRect = player.getBounds();
                for (const auto& atkBound : boss.getAttackBounds()) {
                    if (playerRect.findIntersection(atkBound).has_value()) {

                        player.playSound(4);
                        player.setHealth(player.getHealth() - 1);
                        playerInvulnTimer = 1.5f;
                        break;
                    }
                }
            }

            if (boss.getCurrentPhase() == 3) {
                sf::Vector2f pPos = player.getPosition();
                float cy = 1440.f / 2.f + 100.f;
                float offsetY = 350.f;
                float bottomPlatformY = cy + offsetY;

                if (pPos.y + 180.f > bottomPlatformY + 80.f) {
                    player.setHealth(player.getHealth() - 1);
                    player.playSound(4);
                    playerInvulnTimer = 1.5f;
                    player.resetPositionForPhase(3, 3440.f, 1440.f);
                }

                if (pPos.x < 0.f || pPos.x > 3440.f - 60.f) {
                    player.setHealth(player.getHealth() - 1);
                    player.playSound(4);
                    playerInvulnTimer = 1.5f;
                    player.resetPositionForPhase(3, 3440.f, 1440.f);

                }
            }
            if (boss.getCurrentPhase() == 2 && playerInvulnTimer <= 0.f) {
                sf::FloatRect pb = player.getBounds();
                sf::Vector2f playerCenter(pb.position.x + pb.size.x / 2.f,
                    pb.position.y + pb.size.y / 2.f);

                for (const auto& atk : boss.getAttacks()) {
                    if (!atk.isLaser) continue;

                    sf::Vector2f origin = atk.shape.getPosition();
                    float angleDeg = atk.shape.getRotation().asDegrees();
                    float angleRad = angleDeg * 3.14159f / 180.f;
                    sf::Vector2f dir(std::cos(angleRad), std::sin(angleRad));
                    sf::Vector2f toPlayer = playerCenter - origin;

                    float dist = std::abs(toPlayer.x * (-dir.y) + toPlayer.y * dir.x);
                    
                    if (dist < 30.f) {
                        player.playSound(4);
                        player.setHealth(player.getHealth() - 1);
                        playerInvulnTimer = 1.5f;
                        break;
                    }
                }
            }


            if (boss.checkEarthCrush()) {
                player.setHealth(player.getHealth() - 2);
                playerInvulnTimer = 1.5f;
            }

            if (player.getHealth() <= 0) {
                currentState = GameState::GAME_OVER;
            }

            int phase = boss.getCurrentPhase();
            auto& bossAttacks = boss.getAttacks();

            for (auto& proj : player.getProjectiles()) {
                if (!proj->isActive()) continue;

                if (proj->getType() == 2) {
                    bool blockedByEarth = false;
                    for (auto& atk : bossAttacks) {
                        if (atk.active && atk.type == 2) {
                            if (proj->checkCollision(atk.shape.getGlobalBounds())) {
                                proj->deactivate();
                                blockedByEarth = true;
                                break;
                            }
                        }
                    }
                    if (blockedByEarth) continue;
                }

                if (!proj->isActive()) continue;

                if (!proj->isActive()) continue;

                if (proj->checkCollision(boss.getBounds())) {
                    bool isEffective = false;

                    if (phase == 0 && proj->getType() == 0) isEffective = true;
                    if (phase == 1 && proj->getType() == 3) isEffective = true;
                    if (phase == 2 && proj->getType() == 2) isEffective = true;
                    if (phase == 3 && proj->getType() == 1) isEffective = true;

                    if (isEffective) {

                        boss.takeDamage(proj->getDamage());


                        float baseScore = 100.0f;
                        float pointsToAdd = (baseScore * currentMultiplier * proj->getDamage()) + (currentMultiplier * baseScore);
                        score += pointsToAdd;
                    }

                    if (proj->getType() != 0) {
                        proj->deactivate();
                    }
                }
            }

            gameInterface.update(gameClock, score, player.getHealth(), player.getMaxHealth());
        }

        window.clear(sf::Color(178, 178, 178));

        if (currentState == GameState::MENU) {
            mainMenu.draw(window);
        }
        else if (currentState == GameState::GAME) {

			gameArea.draw(window, boss.getCurrentPhase());
            boss.draw(window);
            player.draw(window);
            gameInterface.draw(window);
        }
        else if (currentState == GameState::GAME_OVER) {
            gameOverMenu.draw(window);
        }
        if (showHitboxes) {
            auto drawDebugBox = [&](const sf::FloatRect& bounds, sf::Color color) {
                sf::RectangleShape box(bounds.size);
                box.setPosition(bounds.position);
                box.setFillColor(sf::Color::Transparent);
                box.setOutlineColor(color);
                box.setOutlineThickness(2.f);
                window.draw(box);
                };

            drawDebugBox(player.getBounds(), sf::Color::Red);
            drawDebugBox(boss.getBounds(), sf::Color::Magenta);

            for (const auto& attackRect : boss.getAttackBounds()) {
                drawDebugBox(attackRect, sf::Color(255, 165, 0));
            }

            for (const auto& proj : player.getProjectiles()) {
                if (proj->isActive()) {
                    drawDebugBox(proj->getBounds(), sf::Color::Green);
                }
            }
        }
        window.display();
    }
    return 0;
}
