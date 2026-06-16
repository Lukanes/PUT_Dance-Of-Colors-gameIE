#include "interface.h"

float GameClock::getElapsedTime() const {
    return clock.getElapsedTime().asSeconds();
}

void GameClock::restart() {
    clock.restart();
}

MainMenu::MainMenu(float w, float h) : width(w), height(h), selectedItemIndex(0), currentResolutionIndex(0) {
    resolutions = { sf::VideoMode({3440, 1440}), sf::VideoMode({2560, 1440}), sf::VideoMode({1920, 1080}) };
}

void MainMenu::loadResources(const sf::Font& fontRef) {
    font = fontRef;
    menu.clear();

    std::string items[3] = { "Play", "Resolution: 3440 x 1440", "Exit" };
    for (int i = 0; i < 3; ++i) {
        sf::Text text(font);
        text.setString(items[i]);
        text.setCharacterSize(50);
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        text.setPosition(sf::Vector2f(width / 2.f - text.getGlobalBounds().size.x / 2.f, height / 2.f + i * 100.f));
        menu.push_back(text);
    }
}

void MainMenu::draw(sf::RenderWindow& window) {
    for (const auto& item : menu) {
        window.draw(item);
    }
}

void MainMenu::moveUp() {
    if (selectedItemIndex - 1 >= 0) {
        menu[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menu[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void MainMenu::moveDown() {
    if (selectedItemIndex + 1 < menu.size()) {
        menu[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menu[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

int MainMenu::getSelectedOption() const {
    return selectedItemIndex;
}

void MainMenu::cycleResolution(const sf::Font& fontRef) {
    currentResolutionIndex = (currentResolutionIndex + 1) % resolutions.size();
    auto res = resolutions[currentResolutionIndex];
    menu[1].setString("Resolution: " + std::to_string(res.size.x) + " x " + std::to_string(res.size.y));
    menu[1].setPosition(sf::Vector2f(width / 2.f - menu[1].getGlobalBounds().size.x / 2.f, menu[1].getPosition().y));
}

sf::VideoMode MainMenu::getCurrentResolution() const {
    return resolutions[currentResolutionIndex];
}

GameOverMenu::GameOverMenu(float w, float h) : width(w), height(h), selectedItemIndex(0), titleText(font) {}

void GameOverMenu::loadResources(const sf::Font& fontRef) {
    font = fontRef;
    menu.clear();

    titleText.setFont(font);
    titleText.setString("GAME OVER");
    titleText.setCharacterSize(100);
    titleText.setFillColor(sf::Color::Red);
    titleText.setPosition(sf::Vector2f(width / 2.f - titleText.getGlobalBounds().size.x / 2.f, height / 3.f));

    std::string items[2] = { "Try Again", "Main Menu" };
    for (int i = 0; i < 2; ++i) {
        sf::Text text(font);
        text.setString(items[i]);
        text.setCharacterSize(50);
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        text.setPosition(sf::Vector2f(width / 2.f - text.getGlobalBounds().size.x / 2.f, height / 2.f + i * 100.f));
        menu.push_back(text);
    }
}

void GameOverMenu::draw(sf::RenderWindow& window) {
    window.draw(titleText);
    for (const auto& item : menu) {
        window.draw(item);
    }
}

void GameOverMenu::moveUp() {
    if (selectedItemIndex - 1 >= 0) {
        menu[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menu[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void GameOverMenu::moveDown() {
    if (selectedItemIndex + 1 < menu.size()) {
        menu[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menu[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

int GameOverMenu::getSelectedOption() const {
    return selectedItemIndex;
}

bool Interface::loadResources() {
    if (!font.openFromFile("assets/font.ttf")) return false;

    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(20.f, 20.f));

    timeText.setFont(font);
    timeText.setCharacterSize(40);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(sf::Vector2f(20.f, 70.f));

    hpBarBg.setSize(sf::Vector2f(300.f, 30.f));
    hpBarBg.setFillColor(sf::Color(50, 50, 50, 200));
    hpBarBg.setPosition(sf::Vector2f(20.f, 130.f));

    hpBarFg.setSize(sf::Vector2f(300.f, 30.f));
    hpBarFg.setFillColor(sf::Color::Green);
    hpBarFg.setPosition(sf::Vector2f(20.f, 130.f));

    return true;
}

const sf::Font& Interface::getFont() const {
    return font;
}

void Interface::update(const GameClock& gameClock, float score, float playerHealth, float maxHealth) {
    scoreText.setString("Score: " + std::to_string(static_cast<int>(score)));

    float elapsed = gameClock.getElapsedTime();
    int minutes = static_cast<int>(elapsed) / 60;
    int seconds = static_cast<int>(elapsed) % 60;

    char timeBuf[16];
    snprintf(timeBuf, sizeof(timeBuf), "Time: %02d:%02d", minutes, seconds);
    timeText.setString(timeBuf);

    float hpRatio = playerHealth / maxHealth;
    if (hpRatio < 0.f) hpRatio = 0.f;
    hpBarFg.setSize(sf::Vector2f(300.f * hpRatio, 30.f));
}

void Interface::draw(sf::RenderWindow& window) {
    window.draw(scoreText);
    window.draw(timeText);
    window.draw(hpBarBg);
    window.draw(hpBarFg);
}