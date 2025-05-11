#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <mutex>

#include "PhysicalObject.hpp"

class Ball : public PhysicalObject {
   private:
    // Position and physics data
    sf::Vector2f m_velocity;
    sf::Vector2f m_lastPosition;
    sf::Vector2f m_pixelVelocity;
    float m_radius;

    // Display data
    sf::CircleShape m_shape;
    sf::Color m_baseColor;

    // State flags
    bool m_atRest = false;
    mutable std::mutex m_mutex;

    void handleWallCollision(const sf::Vector2f& windowSize);
    void updateColor();

   public:
    Ball(float radius, const sf::Vector2f& pos, const sf::Vector2f& vel,
         const sf::Color& color);

    void update(float dt, const sf::Vector2f& windowSize) override;
    void draw(sf::RenderWindow& window) override;
    void applyImpulse(const sf::Vector2f& impulse) override;
    void resolveCollision(Ball& other);

    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    bool isAtRest() const;
    sf::Vector2f getPixelVelocity() const { return m_pixelVelocity; }
    float getRadius() const { return m_radius; }
    sf::Color getColor() const { return m_shape.getFillColor(); }
};
