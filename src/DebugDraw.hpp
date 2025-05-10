#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Ball.hpp"

class DebugDraw {
   public:
    static void drawLine(sf::RenderWindow& window, const sf::Vector2f& start,
                         const sf::Vector2f& direction, float length,
                         const sf::Color& color);
    static void drawDirectionLine(sf::RenderWindow& window, const Ball* ball);
    static void drawVelocityLine(sf::RenderWindow& window, const Ball* ball);
};
