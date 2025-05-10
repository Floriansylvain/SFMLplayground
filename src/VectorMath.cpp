#include "VectorMath.hpp"

#include <cmath>

namespace VectorMath {
float length(const sf::Vector2f& vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f normalize(const sf::Vector2f& vector) {
    float len = length(vector);
    if (len > 0.0001f) {
        return vector / len;
    }
    return vector;
}
}  // namespace VectorMath
