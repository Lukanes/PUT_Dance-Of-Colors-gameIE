#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    sf::RenderWindow window(sf::VideoMode({200, 200}), "Hello From SFML");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Magenta);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}