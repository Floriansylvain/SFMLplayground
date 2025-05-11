#pragma once
#include <SFML/Graphics/CircleShape.hpp>

#include "PhysicalObject.hpp"

class Ball : public PhysicalObject {
   private:
    sf::CircleShape m_shape;
    sf::Vector2f m_velocity;
    float m_radius;
    bool m_atRest = false;
    sf::Vector2f m_lastPosition;
    sf::Vector2f m_pixelVelocity;

    void handleWallCollision();

   public:
    Ball(float radius, const sf::Vector2f& pos, const sf::Vector2f& vel,
         const sf::Color& color);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void applyImpulse(const sf::Vector2f& impulse) override;
    void resolveCollision(Ball& other);

    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    bool isAtRest() const;
    sf::Vector2f getPixelVelocity() const { return m_pixelVelocity; }
};
