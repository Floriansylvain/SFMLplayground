#include "BatchRenderer.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

BatchRenderer::BatchRenderer() : m_vertices(sf::PrimitiveType::Triangles) {
  constexpr unsigned int size = 64;
  sf::Image image(sf::Vector2u(size, size), sf::Color::Transparent);

  constexpr unsigned int radius = size / 2;
  constexpr sf::Vector2u center(radius, radius);

  for (unsigned int y = 0; y < size; ++y) {
    for (unsigned int x = 0; x < size; ++x) {
      const sf::Vector2u pixel(x, y);
      const int dx = static_cast<int>(center.x) - static_cast<int>(pixel.x);
      const int dy = static_cast<int>(center.y) - static_cast<int>(pixel.y);

      if (const auto distance =
              static_cast<float>(std::sqrt(dx * dx + dy * dy));
          distance <= radius) {
        float alpha = 255.0f;
        if (distance > radius - 2.0f) {
          alpha = 255.0f * (1.0f - (distance - (radius - 2.0f)) / 2.0f);
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

void BatchRenderer::addBall(const Ball &ball) {
  const sf::Vector2f position = ball.getPosition();
  const float radius = ball.getRadius();
  const sf::Color color = ball.getColor();

  sf::Vertex vertices[4];
  vertices[0].position = sf::Vector2f(position.x - radius, position.y - radius);
  vertices[1].position = sf::Vector2f(position.x + radius, position.y - radius);
  vertices[2].position = sf::Vector2f(position.x + radius, position.y + radius);
  vertices[3].position = sf::Vector2f(position.x - radius, position.y + radius);

  const float xSize = static_cast<float>(m_circleTexture.getSize().x);
  const float ySize = static_cast<float>(m_circleTexture.getSize().y);
  vertices[0].texCoords = sf::Vector2f(0, 0);
  vertices[1].texCoords = sf::Vector2f(xSize, 0);
  vertices[2].texCoords = sf::Vector2f(xSize, ySize);
  vertices[3].texCoords = sf::Vector2f(0, ySize);

  for (auto &v : vertices) v.color = color;

  m_vertices.append(vertices[0]);
  m_vertices.append(vertices[1]);
  m_vertices.append(vertices[2]);

  m_vertices.append(vertices[0]);
  m_vertices.append(vertices[2]);
  m_vertices.append(vertices[3]);
}

void BatchRenderer::draw(sf::RenderWindow &window) const {
  if (m_vertices.getVertexCount() == 0) return;

  sf::RenderStates states;
  states.texture = &m_circleTexture;

  window.draw(m_vertices, states);
}
