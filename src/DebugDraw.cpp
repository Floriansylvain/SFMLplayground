#include "DebugDraw.hpp"

#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>

#include "VectorMath.hpp"

float clamp(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

void DebugDraw::drawLine(sf::RenderWindow& window, const sf::Vector2f& start,
                         const sf::Vector2f& direction, float length,
                         const sf::Color& color) {
    sf::Vector2f endPoint = start + VectorMath::normalize(direction) * length;
    sf::Vertex line[] = {{start, color}, {endPoint, color}};

    window.draw(line, 2, sf::PrimitiveType::Lines);
}

void DebugDraw::drawDirectionLine(sf::RenderWindow& window, const Ball* ball) {
    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld(static_cast<float>(mousePixel.x),
                            static_cast<float>(mousePixel.y));
    sf::Vector2f dir = mouseWorld - ballCenter;

    drawLine(window, ballCenter, dir, 100.f, sf::Color::Green);
}

void DebugDraw::drawVelocityLine(sf::RenderWindow& window, const Ball* ball) {
    if (ball->isAtRest()) return;

    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2f velocity = ball->getVelocity();
    float velLength = VectorMath::length(velocity);
    float clampedLength = std::max(0.f, std::min(velLength, 100.f));

    drawLine(window, ballCenter, velocity, clampedLength, sf::Color::Blue);
}
