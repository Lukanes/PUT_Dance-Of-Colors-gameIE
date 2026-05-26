#include <SFML/Graphics.hpp>
#include <optional>
#include "Game/interface/score/clock/clock.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Timer Example");
    window.setFramerateLimit(60);

    // 1. Inicjalizacja Twojego zegara
    GameClock gameClock;

    // 2. Ładowanie czcionki (SFML wymaga czcionki do wyświetlenia tekstu)
    sf::Font font;
    if (!font.openFromFile("Game/fonts/Warnes-Regular.ttf")) { // Upewnij się, że masz plik arial.ttf w folderze z projektem!
        std::cerr << "Nie udalo sie zaladowac czcionki!" << std::endl;
        return -1;
    }

    // 3. Konfiguracja obiektu tekstowego SFML
    sf::Text timerText(font);
    timerText.setCharacterSize(30);            // Rozmiar w pikselach
    timerText.setFillColor(sf::Color::White);  // Kolor tekstu
    timerText.setPosition({10.f, 10.f});         // Pozycja w lewym górnym rogu ekranu

    // Główna pętla gry
    while (window.isOpen()) {
        // Główna pętla zdarzeń w SFML 3
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Zamknięcie okna
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Wciśnięcie klawisza R
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    gameClock.reset();
                }
            }
        }


        // 4. AKTUALIZACJA: Pobieramy string z Twojej klasy i ustawiamy go w SFML
        timerText.setString(gameClock.getTimeStr());

        // 5. RENDEROWANIE
        window.clear(sf::Color::Black);
        
        window.draw(timerText); // Rysowanie timera na ekranie
        
        window.display();
    }

    return 0;
}