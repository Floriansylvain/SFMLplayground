#include "VectorMath.hpp"

#include <cmath>

namespace VectorMath {
float length(const sf::Vector2f& vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f normalize(const sf::Vector2f& vector) {
    float squaredLen = vector.x * vector.x + vector.y * vector.y;
    if (squaredLen > 0.0001f) {
        float invLen = 1.0f / std::sqrt(squaredLen);
        return sf::Vector2f(vector.x * invLen, vector.y * invLen);
    }
    return vector;
}
}  // namespace VectorMath
