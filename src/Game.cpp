#include "Game.hpp"

#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <thread>
#include <unordered_map>

#include "BallFactory.hpp"
#include "Constants.hpp"
#include "DebugDraw.hpp"
#include "ThreadUtils.hpp"
#include "VectorMath.hpp"

Game::Game()
    : m_inputManager(
          [this](auto &&PH1) {
            processKeyPressed(std::forward<decltype(PH1)>(PH1));
          },
          [this](auto &&PH1) {
            processMousePressed(std::forward<decltype(PH1)>(PH1));
          }),
      m_debugLines(sf::PrimitiveType::Lines),
      m_debugOverlay("assets/consolas.ttf"),
      m_threadPool(ThreadUtils::calculateSafeWorkerThreads()) {
  m_window.create(sf::VideoMode({Constants::WIDTH, Constants::HEIGHT}),
                  "SFML Playground");
  m_window.setVerticalSyncEnabled(false);
  m_windowSize = sf::Vector2f(Constants::WIDTH, Constants::HEIGHT);
  m_objects.clear();

  auto balls = BallFactory::generateBalls();
  for (auto &ball : balls) {
    m_objects.push_back(std::move(ball));
  }
}

void Game::processKeyPressed(const sf::Event::KeyPressed &keyPressed) {
  const auto mousePos = sf::Mouse::getPosition(m_window);
  switch (keyPressed.code) {
    case sf::Keyboard::Key::Equal:
      m_timeScale = std::min(10.0f, m_timeScale + 0.25f);
      break;
    case sf::Keyboard::Key::Hyphen:
      m_timeScale = std::max(0.25f, m_timeScale - 0.25f);
      break;
    case sf::Keyboard::Key::D:
      m_toggleDebug = !m_toggleDebug;
      break;
    case sf::Keyboard::Key::Delete:
      if (!m_objects.empty()) m_objects.pop_back();
      break;
    case sf::Keyboard::Key::Insert:
      spawnBall(sf::Vector2f(static_cast<float>(mousePos.x),
                             static_cast<float>(mousePos.y)));
      break;
    default:
      break;
  }
}

void Game::processMousePressed(
    const sf::Event::MouseButtonPressed &mousePressed) {
  const sf::Vector2f mousePos(static_cast<float>(mousePressed.position.x),
                              static_cast<float>(mousePressed.position.y));
  switch (mousePressed.button) {
    case sf::Mouse::Button::Left:
      impulseBalls(mousePos);
      break;
    case sf::Mouse::Button::Right:
      spawnBall(mousePos);
      break;
    default:
      break;
  }
}

void Game::spawnBall(const sf::Vector2f &mousePos) {
  auto newBallPtr =
      std::make_unique<Ball>(Constants::BALL_RADIUS, mousePos,
                             sf::Vector2f(0.f, 0.f), sf::Color::Black);
  m_objects.push_back(std::move(newBallPtr));
}

void Game::impulseBalls(const sf::Vector2f &mousePos) const {
  for (auto &object : m_objects) {
    auto *ball = dynamic_cast<Ball *>(object.get());
    if (!ball) continue;

    sf::Vector2f dir = mousePos - ball->getPosition();
    ball->applyImpulse(Constants::IMPULSE * VectorMath::normalize(dir));
  }
}

Game::Grid Game::buildSpatialGrid() const {
  constexpr float safeCellSize = std::max(2 * Constants::BALL_RADIUS, 0.001f);
  Grid grid;

  for (auto &object : m_objects) {
    auto ball = dynamic_cast<Ball *>(object.get());
    if (!ball) continue;

    const sf::Vector2f &pos = ball->getPosition();
    if (!std::isfinite(pos.x) || !std::isfinite(pos.y)) continue;

    int cellX = static_cast<int>(std::floor(pos.x / safeCellSize));
    int cellY = static_cast<int>(std::floor(pos.y / safeCellSize));
    grid[{cellX, cellY}].push_back(ball);
  }
  return grid;
}

void Game::resolveSpatialCollisionsParallel(const Grid &grid) {
  static const Cell forwardNeighbors[] = {
      {0, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};

  std::vector<std::pair<Ball *, Ball *>> collisionPairs;
  std::mutex collisionsMutex;

  for (const auto &[cell, cellBalls] : grid) {
    for (const auto &[firstOffset, secondOffset] : forwardNeighbors) {
      Cell neighborCell = {cell.first + firstOffset,
                           cell.second + secondOffset};
      auto neighborIt = grid.find(neighborCell);
      if (neighborIt == grid.end()) continue;

      if (neighborCell == cell) {
        for (size_t i = 0; i < cellBalls.size(); ++i) {
          for (size_t j = i + 1; j < cellBalls.size(); ++j) {
            std::lock_guard lock(collisionsMutex);
            collisionPairs.emplace_back(cellBalls[i], cellBalls[j]);
          }
        }
      } else {
        for (Ball *ballA : cellBalls) {
          for (Ball *ballB : neighborIt->second) {
            std::lock_guard lock(collisionsMutex);
            collisionPairs.emplace_back(ballA, ballB);
          }
        }
      }
    }
  }

  const size_t chunkSize =
      std::max(static_cast<size_t>(1),
               collisionPairs.size() / std::thread::hardware_concurrency());
  std::vector<std::future<void>> futures;

  for (size_t i = 0; i < collisionPairs.size(); i += chunkSize) {
    size_t end = std::min(i + chunkSize, collisionPairs.size());

    futures.push_back(m_threadPool.enqueue([&collisionPairs, i, end] {
      for (size_t j = i; j < end; ++j) {
        auto &[ballA, ballB] = collisionPairs[j];
        ballA->resolveCollision(*ballB);
      }
    }));
  }

  for (auto &future : futures) future.get();
}

void Game::update() {
  float dt = m_clock.restart().asSeconds() * m_timeScale;
  if (dt > 0.1f) dt = 0.1f;

  updateBallsParallel(dt);
  resolveSpatialCollisionsParallel(buildSpatialGrid());
}

void Game::updateBallsParallel(float dt) {
  const size_t optimalThreads =
      ThreadUtils::calculateOptimalThreads(m_objects.size());
  if (m_threadPool.getThreadCount() != optimalThreads) {
    m_threadPool.resize(optimalThreads);
  }

  const size_t chunkSize =
      std::max(static_cast<size_t>(1), m_objects.size() / optimalThreads);
  std::vector<std::future<void>> futures;

  for (size_t i = 0; i < m_objects.size(); i += chunkSize) {
    size_t end = std::min(i + chunkSize, m_objects.size());

    futures.push_back(m_threadPool.enqueue([&, i, end, dt] {
      for (size_t j = i; j < end; ++j) {
        if (auto *ball = dynamic_cast<Ball *>(m_objects[j].get())) {
          ball->update(dt, m_windowSize);
        } else {
          m_objects[j]->update(dt, m_windowSize);
        }
      }
    }));
  }

  for (auto &future : futures) future.get();
}

void Game::render() {
  m_window.clear(sf::Color::Black);
  m_debugLines.clear();
  m_drawCallCount = 0;
  m_batchRenderer.clear();

  std::vector<const Ball *> balls;
  balls.reserve(m_objects.size());

  for (const auto &object : m_objects) {
    if (const auto *ball = dynamic_cast<Ball *>(object.get())) {
      balls.push_back(ball);
      m_batchRenderer.addBall(*ball);
    } else {
      object->draw(m_window);
      ++m_drawCallCount;
    }

    if (!m_toggleDebug) continue;
    if (const auto *ball = dynamic_cast<Ball *>(object.get())) {
      // DebugDraw::addDirectionLine(m_debugLines, ball, m_window);
      DebugDraw::addVelocityLine(m_debugLines, ball);
    }
  }

  m_batchRenderer.draw(m_window);
  ++m_drawCallCount;

  if (m_toggleDebug) {
    DebugDraw::drawBatchedLines(m_window, m_debugLines);
    const size_t threadCount = m_threadPool.getThreadCount();
    const size_t ballsPerThread = m_objects.size() / threadCount;
    m_debugOverlay.update(m_drawCallCount + 2, m_timeScale, m_window,
                          threadCount, m_objects.size(), ballsPerThread);
    m_debugOverlay.draw(m_window);
  }

  m_window.display();
}

void Game::handleEvent(const sf::Event &event) {
  if (event.is<sf::Event::Closed>()) {
    m_window.close();
    return;
  }
  if (const auto *resized = event.getIf<sf::Event::Resized>()) {
    constexpr sf::Vector2f position(0.f, 0.f);
    const sf::Vector2f size(static_cast<float>(resized->size.x),
                            static_cast<float>(resized->size.y));
    const sf::FloatRect visibleArea(position, size);
    m_window.setView(sf::View(visibleArea));
    m_windowSize = size;
  }
  if (const auto kP = event.getIf<sf::Event::KeyPressed>())
    processKeyPressed(*kP);
  if (const auto mP = event.getIf<sf::Event::MouseButtonPressed>())
    processMousePressed(*mP);
}

void Game::run() {
  while (m_window.isOpen()) {
    while (const std::optional event = m_window.pollEvent())
      handleEvent(*event);
    update();
    render();
  }
}
