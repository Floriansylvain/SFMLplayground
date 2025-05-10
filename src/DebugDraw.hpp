#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "Ball.hpp"

class DebugDraw {
   public:
    static void addLine(sf::VertexArray& lines, const sf::Vector2f& start,
                        const sf::Vector2f& direction, float length,
                        const sf::Color& color);
    static void addDirectionLine(sf::VertexArray& lines, const Ball* ball,
                                 const sf::RenderWindow& window);
    static void addVelocityLine(sf::VertexArray& lines, const Ball* ball);
    static void drawBatchedLines(sf::RenderWindow& window,
                                 const sf::VertexArray& lines);
};
