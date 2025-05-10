#include "BallFactory.hpp"

#include <cstdlib>
#include <ctime>

#include "Constants.hpp"

namespace BallFactory {
std::unique_ptr<Ball> generateRandBall() {
    float radius = Constants::BALL_RADIUS;
    int diameter = static_cast<int>(2 * radius);
    int maxX = Constants::WIDTH - diameter;
    int maxY = Constants::HEIGHT - diameter;
    float x = static_cast<float>((std::rand() % maxX) + radius);
    float y = static_cast<float>((std::rand() % maxY) + radius);
    float vel = static_cast<float>((std::rand() % 401) - 200);

    auto randColorComp = []() {
        return static_cast<std::uint8_t>(64 + (std::rand() % (256 - 64)));
    };
    sf::Color color(randColorComp(), randColorComp(), randColorComp());

    auto position = sf::Vector2f(x, y);
    auto velocity = sf::Vector2f(vel, vel);
    return std::make_unique<Ball>(radius, position, velocity, color);
}

std::vector<std::unique_ptr<Ball>> generateBalls() {
    std::vector<std::unique_ptr<Ball>> balls;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int i = 0; i < Constants::BALL_QUANTITY; ++i) {
        auto ball = generateRandBall();
        balls.push_back(std::move(ball));
    }
    return balls;
}
}  // namespace BallFactory