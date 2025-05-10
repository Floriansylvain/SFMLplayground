#include "DebugDraw.hpp"

#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>

#include "VectorMath.hpp"

float clamp(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

void DebugDraw::addLine(sf::VertexArray& lines, const sf::Vector2f& start,
                        const sf::Vector2f& direction, float length,
                        const sf::Color& color) {
    sf::Vector2f endPoint = start + VectorMath::normalize(direction) * length;
    lines.append(sf::Vertex({start, color}));
    lines.append(sf::Vertex({endPoint, color}));
}

void DebugDraw::addDirectionLine(sf::VertexArray& lines, const Ball* ball,
                                 const sf::RenderWindow& window) {
    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld(static_cast<float>(mousePixel.x),
                            static_cast<float>(mousePixel.y));
    sf::Vector2f dir = mouseWorld - ballCenter;

    addLine(lines, ballCenter, dir, 100.f, sf::Color::Green);
}

void DebugDraw::addVelocityLine(sf::VertexArray& lines, const Ball* ball) {
    if (ball->isAtRest()) return;

    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2f velocity = ball->getVelocity();
    float velLength = VectorMath::length(velocity);
    float clampedLength = std::max(0.f, std::min(velLength, 100.f));

    addLine(lines, ballCenter, velocity, clampedLength, sf::Color::Blue);
}

void DebugDraw::drawBatchedLines(sf::RenderWindow& window,
                                 const sf::VertexArray& lines) {
    if (lines.getVertexCount() > 0) window.draw(lines);
}
