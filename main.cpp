#include <SFML/Graphics.hpp>
#include <iostream>
#include "clock.h"
#include "interface.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Dance of Colors");
    window.setFramerateLimit(60);

    GameClock gameClock;
    Interface gameInterface;
    int score = 0;

    while (window.isOpen()) {
        std::optional<sf::Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        gameInterface.update(gameClock, score);
        window.clear();
        gameInterface.draw(window);
        window.display();
    }

    return 0;
}