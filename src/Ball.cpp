#include "Ball.hpp"

#include <cmath>

#include "Constants.hpp"

Ball::Ball(const float radius, const sf::Vector2f &pos, const sf::Vector2f &vel,
           const sf::Color &color)
    : m_velocity(vel),
      m_lastPosition(pos),
      m_pixelVelocity(vel),
      m_radius(radius),
      m_baseColor(color) {
  m_shape.setRadius(radius);
  m_shape.setOrigin(sf::Vector2f(radius, radius));
  m_shape.setPosition(pos);
  m_shape.setFillColor(color);
}

void Ball::update(const float dt, const sf::Vector2f &windowSize) {
  if (m_atRest) return;
  m_velocity.y += Constants::GRAVITY * dt;
  m_shape.move(m_velocity * dt);

  const sf::Vector2f currentPosition = m_shape.getPosition();
  m_pixelVelocity = (currentPosition - m_lastPosition) / dt;
  m_lastPosition = currentPosition;

  updateColor();
  handleWallCollision(windowSize);
}

void Ball::updateColor() {
  const float currentSpeed =
      std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

  static float lastSpeed = 0.0f;
  if (std::abs(currentSpeed - lastSpeed) < 10.0f) return;
  lastSpeed = currentSpeed;

  const float speed =
      std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
  const float t = std::min(speed / 2000.0f, 1.0f);

  sf::Color targetColor;

  if (t < 0.33f) {
    // Red to orange
    const float scaledT = t * 3.0f;
    targetColor.r = static_cast<std::uint8_t>(255);
    targetColor.g = static_cast<std::uint8_t>(0 + scaledT * 165);
    targetColor.b = static_cast<std::uint8_t>(0);
  } else if (t < 0.66f) {
    // Orange to yellow
    const float scaledT = (t - 0.33f) * 3.0f;
    targetColor.r = static_cast<std::uint8_t>(255);
    targetColor.g = static_cast<std::uint8_t>(165 + scaledT * 90);
    targetColor.b = static_cast<std::uint8_t>(0);
  } else {
    // Yellow to white
    const float scaledT = (t - 0.66f) * 3.0f;
    targetColor.r = static_cast<std::uint8_t>(255);
    targetColor.g = static_cast<std::uint8_t>(255);
    targetColor.b = static_cast<std::uint8_t>(0 + scaledT * 255);
  }

  const sf::Color currentColor = m_shape.getFillColor();

  constexpr float TRANSITION_SPEED = 0.05f;

  sf::Color newColor;
  newColor.r = static_cast<std::uint8_t>(static_cast<float>(currentColor.r) +
                                         TRANSITION_SPEED *
                                             (targetColor.r - currentColor.r));
  newColor.g = static_cast<std::uint8_t>(static_cast<float>(currentColor.g) +
                                         TRANSITION_SPEED *
                                             (targetColor.g - currentColor.g));
  newColor.b = static_cast<std::uint8_t>(static_cast<float>(currentColor.b) +
                                         TRANSITION_SPEED *
                                             (targetColor.b - currentColor.b));
  newColor.a = 255;

  m_shape.setFillColor(newColor);
}

void Ball::draw(sf::RenderWindow &window) { window.draw(m_shape); }

void Ball::applyImpulse(const sf::Vector2f &impulse) {
  m_velocity += impulse;
  m_atRest = false;
}

sf::Vector2f Ball::getPosition() const { return m_shape.getPosition(); }

sf::Vector2f Ball::getVelocity() const { return m_velocity; }

bool Ball::isAtRest() const { return m_atRest; }

void Ball::handleWallCollision(const sf::Vector2f &windowSize) {
  sf::Vector2f pos = m_shape.getPosition();

  auto handleAxis = [&](const int axis, const float min, const float max,
                        float &velocity, const float radius,
                        const float restitution) {
    float value = axis == 0 ? pos.x : pos.y;

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

  handleAxis(0, 0.f, windowSize.x, m_velocity.x, m_radius,
             Constants::RESTITUTION);
  handleAxis(1, 0.f, windowSize.y, m_velocity.y, m_radius,
             Constants::RESTITUTION);

  if (pos.y + m_radius >= windowSize.y - 1.0f) {
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

void Ball::resolveCollision(Ball &other) {
  std::lock_guard lockA(m_mutex);
  std::lock_guard lockB(other.m_mutex);

  const sf::Vector2f posA = getPosition();
  const sf::Vector2f posB = other.getPosition();
  const sf::Vector2f delta = posB - posA;
  const float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
  const float minDist = m_radius + other.m_radius;

  if (dist >= minDist || dist < 1e-6f) return;

  const sf::Vector2f normal = delta / dist;
  const float overlap = minDist - dist;
  m_shape.move(-normal * (overlap / 2.f));
  other.m_shape.move(normal * (overlap / 2.f));

  const sf::Vector2f vA = m_velocity;
  const sf::Vector2f vB = other.m_velocity;
  const float vA_n = vA.x * normal.x + vA.y * normal.y;
  const float vB_n = vB.x * normal.x + vB.y * normal.y;

  const float vA_n_new = vB_n * Constants::RESTITUTION;
  const float vB_n_new = vA_n * Constants::RESTITUTION;

  m_velocity += (vA_n_new - vA_n) * normal;
  other.m_velocity += (vB_n_new - vB_n) * normal;

  m_atRest = false;
  other.m_atRest = false;
}
