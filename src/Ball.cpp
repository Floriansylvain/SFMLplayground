#include "Ball.hpp"

#include <cmath>

#include "Constants.hpp"

Ball::Ball(float radius, const sf::Vector2f& pos, const sf::Vector2f& vel)
    : m_radius(radius),
      m_velocity(vel),
      m_lastPosition(pos),
      m_pixelVelocity(vel) {
    m_shape.setRadius(radius);
    m_shape.setOrigin(sf::Vector2f(radius, radius));
    m_shape.setPosition(pos);
    m_shape.setFillColor(sf::Color::Red);
}

void Ball::update(float dt) {
    if (m_atRest) return;
    m_velocity.y += Constants::GRAVITY * dt;
    m_shape.move(m_velocity * dt);

    sf::Vector2f currentPosition = m_shape.getPosition();
    m_pixelVelocity = (currentPosition - m_lastPosition) / dt;
    m_lastPosition = currentPosition;

    handleWallCollision();
}

void Ball::draw(sf::RenderWindow& window) { window.draw(m_shape); }

void Ball::applyImpulse(const sf::Vector2f& impulse) {
    m_velocity += impulse;
    m_atRest = false;
}

sf::Vector2f Ball::getPosition() const { return m_shape.getPosition(); }

sf::Vector2f Ball::getVelocity() const { return m_velocity; }

bool Ball::isAtRest() const { return m_atRest; }

void Ball::handleWallCollision() {
    sf::Vector2f pos = m_shape.getPosition();

    auto handleAxis = [&](int axis, float min, float max, float& velocity,
                          float radius, float restitution) {
        float value = (axis == 0) ? pos.x : pos.y;

        if (value - radius < min) {
            value = min + radius;
            velocity = -velocity * restitution;
        } else if (value + radius > max) {
            value = max - radius;
            velocity = -velocity * restitution;
            if (axis == 1 && std::abs(velocity) < 10.f) velocity = 0.f;
        }

        (axis == 0 ? pos.x : pos.y) = value;
    };

    handleAxis(0, 0.f, Constants::WIDTH, m_velocity.x, m_radius,
               Constants::RESTITUTION);
    handleAxis(1, 0.f, Constants::HEIGHT, m_velocity.y, m_radius,
               Constants::RESTITUTION);

    if (pos.y + m_radius >= Constants::HEIGHT - 1.0f) {
        m_velocity.x *= Constants::FRICTION;
        if (std::abs(m_velocity.x) < 5.f) m_velocity.x = 0.f;
        if (std::abs(m_pixelVelocity.x) < Constants::REST_PIXEL_VELOCITY &&
            std::abs(m_pixelVelocity.y) < Constants::REST_PIXEL_VELOCITY) {
            m_velocity = {0.f, 0.f};
            m_atRest = true;
        }
    } else {
        m_atRest = false;
    }

    m_shape.setPosition(pos);
}
