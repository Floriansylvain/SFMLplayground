#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "Ball.hpp"

class BatchRenderer {
  sf::VertexArray m_vertices;
  sf::Texture m_circleTexture;

 public:
  BatchRenderer();

  void clear();

  void addBall(const Ball &ball);

  void draw(sf::RenderWindow &window) const;
};
