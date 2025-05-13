#include "VectorMath.hpp"

#include <cmath>

namespace VectorMath {
float length(const sf::Vector2f &vector) {
  return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f normalize(const sf::Vector2f &vector) {
  if (const float squaredLen = vector.x * vector.x + vector.y * vector.y;
      squaredLen > 0.0001f) {
    const float invLen = 1.0f / std::sqrt(squaredLen);
    return {vector.x * invLen, vector.y * invLen};
  }
  return vector;
}
}  // namespace VectorMath
