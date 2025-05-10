#pragma once
#include <SFML/Graphics/CircleShape.hpp>

#include "PhysicalObject.hpp"

class Ball : public PhysicalObject {
   private:
    sf::CircleShape m_shape;
    sf::Vector2f m_velocity;
    float m_radius;

    void handleWallCollision();

   public:
    Ball(float radius, const sf::Vector2f& pos, const sf::Vector2f& vel);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void applyImpulse(const sf::Vector2f& impulse) override;

    sf::Vector2f getPosition() const;
};
