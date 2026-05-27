#include <SFML/Graphics.hpp>
#include <iostream>
#include "clock.h"
#include "interface.h"
#include "player.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({3440, 1440}), "Dance of Colors");
    window.setFramerateLimit(60);

    GameClock gameClock;
    Interface gameInterface;
    int score = 0;

    if (!gameInterface.loadResources()) {
        std::cerr << "failed to load resources!\n";
        return -1;
    }

    Player player(100.f, 200.f);
    sf::Clock deltaClock;

    while (window.isOpen()) {
        float deltaTime = deltaClock.restart().asSeconds();

        std::optional<sf::Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        player.update(deltaTime, 3440.f, 1400.f);
        gameInterface.update(gameClock, score);

        window.clear(sf::Color(178, 178, 178));
        player.draw(window);
        gameInterface.draw(window);
        window.display();
    }

    return 0;
}