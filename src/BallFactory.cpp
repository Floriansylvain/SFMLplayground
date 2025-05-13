#include "BallFactory.hpp"

#include <random>

#include "Constants.hpp"

namespace BallFactory {
std::unique_ptr<Ball> generateRandBall() {
  static std::random_device rd;
  static std::mt19937 rng(rd());
  constexpr float x1 = Constants::WIDTH - Constants::BALL_RADIUS * 2;
  constexpr float y1 = Constants::HEIGHT - Constants::BALL_RADIUS * 2;
  std::uniform_real_distribution posXDist(Constants::BALL_RADIUS, x1);
  std::uniform_real_distribution posYDist(Constants::BALL_RADIUS, y1);
  std::uniform_real_distribution velDist(-200.0f, 200.0f);
  std::uniform_int_distribution colorDist(64, 255);

  float radius = Constants::BALL_RADIUS;
  const float x = posXDist(rng);
  const float y = posYDist(rng);
  const float vel = velDist(rng);

  sf::Color color(static_cast<std::uint8_t>(colorDist(rng)),
                  static_cast<std::uint8_t>(colorDist(rng)),
                  static_cast<std::uint8_t>(colorDist(rng)));

  auto position = sf::Vector2f(x, y);
  auto velocity = sf::Vector2f(vel, vel);
  return std::make_unique<Ball>(radius, position, velocity, color);
}

std::vector<std::unique_ptr<Ball> > generateBalls() {
  std::vector<std::unique_ptr<Ball> > balls;

  for (int i = 0; i < Constants::BALL_QUANTITY; ++i) {
    auto ball = generateRandBall();
    balls.push_back(std::move(ball));
  }
  return balls;
}
}  // namespace BallFactory
