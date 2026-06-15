#pragma once
#include <SFML/Graphics.hpp>

struct Animation {
    sf::Vector2i frameSize;
    int frameCount = 0;
    float frameTime = 0.f;
    float timer = 0.f;
    int currentFrame = 0;

    void update(float dt) {
        timer += dt;
        if (timer >= frameTime) {
            timer = 0.f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    sf::IntRect getRect() {
        return sf::IntRect({ currentFrame * frameSize.x, 0 }, frameSize);
    }
};