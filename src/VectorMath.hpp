#pragma once
#include <SFML/System/Vector2.hpp>

namespace VectorMath {
float length(const sf::Vector2f &vector);

sf::Vector2f normalize(const sf::Vector2f &vector);
}  // namespace VectorMath
