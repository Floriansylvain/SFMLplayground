#pragma once
#include <memory>
#include <vector>

#include "Ball.hpp"

namespace BallFactory {
std::unique_ptr<Ball> generateRandBall();

std::vector<std::unique_ptr<Ball> > generateBalls();
}  // namespace BallFactory
