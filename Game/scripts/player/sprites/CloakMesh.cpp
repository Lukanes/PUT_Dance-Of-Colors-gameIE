#include "CloakMesh.h"
#include <cmath>
#include <cstdint>

static constexpr float PI = 3.14159265f;

CloakMesh::CloakMesh() {
    m_currentColor = sf::Color(128, 128, 128);
}

void CloakMesh::setCurrentColor(sf::Color color) {
    m_currentColor = color;
}

void CloakMesh::setColorFunction(CloakColorFn fn) { m_colorFn = std::move(fn); }

void CloakMesh::initialize(int segments, float width, float height) {
    m_segments = segments;
    m_width = width;
    m_height = height;

    m_fill.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    m_inner.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    m_folds.setPrimitiveType(sf::PrimitiveType::TriangleStrip);

    std::size_t vc = static_cast<std::size_t>((segments + 1) * 2);
    m_fill.resize(vc);
    m_inner.resize(vc);
    m_folds.resize(vc);

    m_hemX.assign(segments + 1, 0.f);
    m_hemY.assign(segments + 1, height);
    m_hemVX.assign(segments + 1, 0.f);
    m_hemVY.assign(segments + 1, 0.f);
    for (int i = 0; i <= segments; ++i)
        m_hemX[i] = (static_cast<float>(i) / segments) * width;
}

void CloakMesh::update(float deltaTime, sf::Vector2f velocity) {
    m_time += deltaTime * 2.2f;

    float targetWindY = -velocity.y * 0.09f;
    float targetWindX = velocity.x * 0.09f;
    float s = 1.f - std::exp(-deltaTime * 3.f);
    m_windY += (targetWindY - m_windY) * s;
    m_windX += (targetWindX - m_windX) * s;

    float narrowing = std::max(0.f, std::min(1.f, m_windY / (m_height * 0.6f)));

    const float stiffness = 12.f;
    const float damping = 5.5f;

    for (int i = 0; i <= m_segments; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(m_segments);

        float fullX = t * m_width - m_windX;
        float restX = fullX + (m_width * 0.5f - fullX) * narrowing * 0.75f;

        float wave = std::sin(t * PI * 2.f - m_time) * m_height * 0.04f;
        float restY = m_height + wave - m_windY;

        float fx = (restX - m_hemX[i]) * stiffness - m_hemVX[i] * damping;
        float fy = (restY - m_hemY[i]) * stiffness - m_hemVY[i] * damping;
        m_hemVX[i] += fx * deltaTime;
        m_hemVY[i] += fy * deltaTime;
        m_hemX[i] += m_hemVX[i] * deltaTime;
        m_hemY[i] += m_hemVY[i] * deltaTime;
    }

    float swing = std::max(-1.f, std::min(1.f, -m_windX / (m_width * 0.25f)));

    auto getOuterColor = [&](float t, float v) -> sf::Color {
        uint8_t r = static_cast<uint8_t>(m_currentColor.r * (1.f - 0.4f * v));
        uint8_t g = static_cast<uint8_t>(m_currentColor.g * (1.f - 0.4f * v));
        uint8_t b = static_cast<uint8_t>(m_currentColor.b * (1.f - 0.4f * v));
        return { r, g, b, 255 };
        };

    auto getInnerColor = [&](float t, float v) -> sf::Color {
        uint8_t r = static_cast<uint8_t>(m_currentColor.r * (0.4f - 0.15f * v));
        uint8_t g = static_cast<uint8_t>(m_currentColor.g * (0.4f - 0.15f * v));
        uint8_t b = static_cast<uint8_t>(m_currentColor.b * (0.4f - 0.15f * v));
        return { r, g, b, 255 };
        };

    for (int i = 0; i <= m_segments; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(m_segments);
        float topSpread = (m_width * 0.52f) * (1.f - narrowing * 0.2f);
        float xTop = m_width * 0.5f + (t - 0.5f) * topSpread;
        float yTop = -std::sin(t * PI) * m_height * 0.08f;

        float bellFlare = m_width * 0.38f * (1.f - narrowing * 0.85f);
        float xBot = m_hemX[i] + (t - 0.5f) * bellFlare;
        float yBot = m_hemY[i];

        m_fill[static_cast<std::size_t>(i * 2 + 0)] = { {xTop, yTop}, getOuterColor(t, 0.f) };
        m_fill[static_cast<std::size_t>(i * 2 + 1)] = { {xBot, yBot}, getOuterColor(t, 1.f) };

        float peekAmt = -swing * m_width * 0.20f;
        float xInBot = xBot + peekAmt;
        float yInBot = yBot - m_height * 0.03f * std::abs(swing);
        m_inner[static_cast<std::size_t>(i * 2 + 0)] = { {xTop,  yTop},  getInnerColor(t, 0.f) };
        m_inner[static_cast<std::size_t>(i * 2 + 1)] = { {xInBot,yInBot},getInnerColor(t, 1.f) };

        float edgeBright = (swing > 0.f) ? t : (1.f - t);
        edgeBright = std::pow(edgeBright, 8.f);
        uint8_t ea = static_cast<uint8_t>(140 * edgeBright);
        sf::Color edgeCol{ m_currentColor.r, m_currentColor.g, m_currentColor.b, ea };
        m_folds[static_cast<std::size_t>(i * 2 + 0)] = { {xTop, yTop}, edgeCol };
        m_folds[static_cast<std::size_t>(i * 2 + 1)] = { {xBot, yBot}, edgeCol };
    }
}

void CloakMesh::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_inner, states);
    target.draw(m_fill, states);
    target.draw(m_folds, states);
}