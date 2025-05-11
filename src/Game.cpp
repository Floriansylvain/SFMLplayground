#include "Game.hpp"

#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <unordered_map>

#include "BallFactory.hpp"
#include "Constants.hpp"
#include "DebugDraw.hpp"
#include "VectorMath.hpp"

Game::Game()
    : m_inputManager(
          std::bind(&Game::processKeyPressed, this, std::placeholders::_1),
          std::bind(&Game::processMousePressed, this, std::placeholders::_1)),
      m_debugLines(sf::PrimitiveType::Lines),
      m_drawCallCount(0),
      m_debugOverlay("assets/consolas.ttf") {
    m_window.create(sf::VideoMode({Constants::WIDTH, Constants::HEIGHT}),
                    "SFML Playground");
    m_window.setVerticalSyncEnabled(true);
    m_objects.clear();

    auto balls = BallFactory::generateBalls();
    for (auto &ball : balls) {
        m_objects.push_back(std::move(ball));
    }
}

void Game::processKeyPressed(const sf::Event::KeyPressed &kP) {
    if (kP.code == sf::Keyboard::Key::Equal) {
        m_timeScale += 0.25f;
        if (m_timeScale > 10.0f) m_timeScale = 10.0f;
    } else if (kP.code == sf::Keyboard::Key::Hyphen) {
        m_timeScale -= 0.25f;
        if (m_timeScale < 0.25f) m_timeScale = 0.25f;
    }

    if (kP.code == sf::Keyboard::Key::F3) {
        m_toggleDebug = !m_toggleDebug;
    }
}

void Game::processMousePressed(const sf::Event::MouseButtonPressed &mP) {
    if (mP.button != sf::Mouse::Button::Left) return;
    handleMouseClick(sf::Vector2i(mP.position.x, mP.position.y));
}

void Game::handleMouseClick(const sf::Vector2i &mousePos) {
    sf::Vector2f mouseWorld(static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y));
    for (auto &object : m_objects) {
        auto *ball = dynamic_cast<Ball *>(object.get());
        if (!ball) continue;

        sf::Vector2f dir = mouseWorld - ball->getPosition();
        ball->applyImpulse(Constants::IMPULSE * VectorMath::normalize(dir));
    }
}

Game::Grid Game::buildSpatialGrid() {
    const float cellSize = 2 * Constants::BALL_RADIUS;
    const float safeCellSize = std::max(cellSize, 0.001f);
    Grid grid;
    for (auto &object : m_objects) {
        if (Ball *ball = dynamic_cast<Ball *>(object.get())) {
            const sf::Vector2f &pos = ball->getPosition();
            if (std::isfinite(pos.x) && std::isfinite(pos.y)) {
                int cellX = static_cast<int>(std::floor(pos.x / safeCellSize));
                int cellY = static_cast<int>(std::floor(pos.y / safeCellSize));
                grid[{cellX, cellY}].push_back(ball);
            }
        }
    }
    return grid;
}

void Game::resolveSpatialCollisions(const Grid &grid) {
    static const Cell forwardNeighbors[] = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};
    for (const auto &[cell, cellBalls] : grid) {
        for (const auto &offset : forwardNeighbors) {
            Cell neighborCell = {cell.first + offset.first,
                                 cell.second + offset.second};
            auto neighborIt = grid.find(neighborCell);
            if (neighborIt == grid.end()) continue;
            if (neighborCell == cell) {
                for (size_t i = 0; i < cellBalls.size(); ++i) {
                    for (size_t j = i + 1; j < cellBalls.size(); ++j) {
                        cellBalls[i]->resolveCollision(*cellBalls[j]);
                    }
                }
            } else {
                for (Ball *ballA : cellBalls) {
                    for (Ball *ballB : neighborIt->second) {
                        ballA->resolveCollision(*ballB);
                    }
                }
            }
        }
    }
}

void Game::update() {
    float dt = m_clock.restart().asSeconds() * m_timeScale;
    if (dt > 0.1f) dt = 0.1f;

    for (auto &object : m_objects) {
        object->update(dt);
    }

    auto grid = buildSpatialGrid();
    resolveSpatialCollisions(grid);
}

void Game::render() {
    m_window.clear(sf::Color::Black);

    m_debugLines.clear();
    m_drawCallCount = 0;

    for (const auto &object : m_objects) {
        object->draw(m_window);
        ++m_drawCallCount;

        if (!m_toggleDebug) continue;
        if (auto *ball = dynamic_cast<Ball *>(object.get())) {
            DebugDraw::addDirectionLine(m_debugLines, ball, m_window);
            DebugDraw::addVelocityLine(m_debugLines, ball);
        }
    }

    if (m_toggleDebug) {
        DebugDraw::drawBatchedLines(m_window, m_debugLines);
        m_debugOverlay.update(m_drawCallCount + 2, m_timeScale, m_window);
        m_debugOverlay.draw(m_window);
    }

    m_window.display();
}

void Game::run() {
    while (m_window.isOpen()) {
        m_inputManager.processEvents(m_window);
        update();
        render();
    }
}
