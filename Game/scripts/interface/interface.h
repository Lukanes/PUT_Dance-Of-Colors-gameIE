#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class GameClock {
private:
    sf::Clock clock;
public:
    float getElapsedTime() const;
    void restart();
};

class MainMenu {
private:
    int selectedItemIndex;
    sf::Font font;
    std::vector<sf::Text> menu;
    std::vector<sf::VideoMode> resolutions;
    int currentResolutionIndex;
    float width;
    float height;

public:
    MainMenu(float width, float height);
    void loadResources(const sf::Font& fontRef);
    void draw(sf::RenderWindow& window);
    void moveUp();
    void moveDown();
    int getSelectedOption() const;
    void cycleResolution(const sf::Font& fontRef);
    sf::VideoMode getCurrentResolution() const;
};

class GameOverMenu {
private:
    int selectedItemIndex;
    sf::Font font;
    std::vector<sf::Text> menu;
    sf::Text titleText;
    float width;
    float height;

public:
    GameOverMenu(float width, float height);
    void loadResources(const sf::Font& fontRef);
    void draw(sf::RenderWindow& window);
    void moveUp();
    void moveDown();
    int getSelectedOption() const;
};

class Interface {
private:
    sf::Font font;
    sf::Text scoreText;
    sf::Text timeText;
    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBarFg;

public:
    Interface() : scoreText(font), timeText(font) {}
    bool loadResources();
    const sf::Font& getFont() const;
    void update(const GameClock& gameClock, float score, float playerHealth, float maxHealth);
    void draw(sf::RenderWindow& window);
};