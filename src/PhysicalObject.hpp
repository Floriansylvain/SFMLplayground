#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

class PhysicalObject {
 public:
  virtual ~PhysicalObject() = default;

  virtual void update(float dt, const sf::Vector2f &windowSize) = 0;

  virtual void draw(sf::RenderWindow &window) = 0;

  virtual void applyImpulse(const sf::Vector2f &impulse) = 0;
};
