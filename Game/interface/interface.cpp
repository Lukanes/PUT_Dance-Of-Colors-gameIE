#include "interface.h"

Interface::Interface() : clockText(font), scoreText(font) {}

bool Interface::loadResources() {
    if (!font.openFromFile("fonts/Warnes-Regular.ttf")) {
        return false;
    }

    clockText.setCharacterSize(24);
    clockText.setFillColor(sf::Color::White);
    clockText.setPosition({10.f, 10.f});

    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({10.f, 40.f});

    return true;
}

void Interface::update(const GameClock& gameClock, int score) {
    clockText.setString("Time: " + gameClock.getTimeStr());
    scoreText.setString("Score: " + std::to_string(score));
}

void Interface::draw(sf::RenderWindow& window) {
    window.draw(clockText);
    window.draw(scoreText);
}