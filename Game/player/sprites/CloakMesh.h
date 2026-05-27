#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <cstdint>

using CloakColorFn = std::function<sf::Color(float t, float v)>;

class CloakMesh : public sf::Drawable, public sf::Transformable {
public:
    CloakMesh();

    void initialize(int segments, float width, float height);
    void update(float deltaTime, sf::Vector2f velocity);
    void setColorFunction(CloakColorFn fn);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    int   m_segments = 0;
    float m_width    = 0.f;
    float m_height   = 0.f;
    float m_time     = 0.f;
    float m_windY    = 0.f;
    float m_windX    = 0.f;

    CloakColorFn    m_colorFn;
    sf::VertexArray m_fill;   // outer bright layer
    sf::VertexArray m_inner;  // dark inner lining
    sf::VertexArray m_folds;  // edge highlight strip

    std::vector<float> m_hemX;
    std::vector<float> m_hemY;
    std::vector<float> m_hemVX;
    std::vector<float> m_hemVY;
};