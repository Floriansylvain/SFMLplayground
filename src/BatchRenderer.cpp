#include "BatchRenderer.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

BatchRenderer::BatchRenderer() : m_vertices(sf::PrimitiveType::Triangles) {
    const unsigned int size = 64;
    sf::Image image(sf::Vector2u(size, size), sf::Color::Transparent);

    unsigned int radius = size / 2;
    sf::Vector2u center(radius, radius);

    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            sf::Vector2u pixel(x, y);
            int dx = static_cast<int>(center.x) - static_cast<int>(pixel.x);
            int dy = static_cast<int>(center.y) - static_cast<int>(pixel.y);
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= radius) {
                float alpha = 255.0f;
                if (distance > radius - 2.0f) {
                    alpha =
                        255.0f * (1.0f - (distance - (radius - 2.0f)) / 2.0f);
                }
                image.setPixel(
                    sf::Vector2u(x, y),
                    sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
            }
        }
    }

    if (!m_circleTexture.loadFromImage(image)) {
        throw std::runtime_error("Failed to load circle texture from image");
    }
}

void BatchRenderer::clear() { m_vertices.clear(); }

void BatchRenderer::addBall(const Ball& ball) {
    sf::Vector2f position = ball.getPosition();
    float radius = ball.getRadius();
    sf::Color color = ball.getColor();

    sf::Vertex topLeft;
    sf::Vertex topRight;
    sf::Vertex bottomRight;
    sf::Vertex bottomLeft;

    topLeft.position = sf::Vector2f(position.x - radius, position.y - radius);
    topRight.position = sf::Vector2f(position.x + radius, position.y - radius);
    bottomRight.position =
        sf::Vector2f(position.x + radius, position.y + radius);
    bottomLeft.position =
        sf::Vector2f(position.x - radius, position.y + radius);

    topLeft.texCoords = sf::Vector2f(0, 0);
    topRight.texCoords = sf::Vector2f(m_circleTexture.getSize().x, 0);
    bottomRight.texCoords =
        sf::Vector2f(m_circleTexture.getSize().x, m_circleTexture.getSize().y);
    bottomLeft.texCoords = sf::Vector2f(0, m_circleTexture.getSize().y);

    topLeft.color = color;
    topRight.color = color;
    bottomRight.color = color;
    bottomLeft.color = color;

    m_vertices.append(topLeft);
    m_vertices.append(topRight);
    m_vertices.append(bottomRight);

    m_vertices.append(topLeft);
    m_vertices.append(bottomRight);
    m_vertices.append(bottomLeft);
}

void BatchRenderer::draw(sf::RenderWindow& window) {
    if (m_vertices.getVertexCount() == 0) return;

    sf::RenderStates states;
    states.texture = &m_circleTexture;

    window.draw(m_vertices, states);
}
