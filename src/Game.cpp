#include "Game.hpp"

#include <SFML/Window/Event.hpp>
#include <cmath>

#include "Constants.hpp"
#include "DebugDraw.hpp"
#include "VectorMath.hpp"

Game::Game()
    : m_inputManager(
          std::bind(&Game::processKeyPressed, this, std::placeholders::_1),
          std::bind(&Game::processMousePressed, this, std::placeholders::_1)) {
    m_window.create(sf::VideoMode({Constants::WIDTH, Constants::HEIGHT}),
                    "SFML Playground");
    m_window.setVerticalSyncEnabled(true);
    m_timeScale = 1.0f;
    m_objects.clear();
    m_objects.push_back(std::make_unique<Ball>(
        Constants::BALL_RADIUS,
        sf::Vector2f(Constants::WIDTH / 2.f, Constants::HEIGHT / 4.f),
        sf::Vector2f(400.f, 0.f)));
}

void Game::processKeyPressed(const sf::Event::KeyPressed &kP) {
    if (kP.code == sf::Keyboard::Key::Equal) {
        m_timeScale *= 1.1f;
    } else if (kP.code == sf::Keyboard::Key::Hyphen) {
        m_timeScale /= 1.1f;
        if (m_timeScale < 0.1f) m_timeScale = 0.1f;
    }
}

void Game::processMousePressed(const sf::Event::MouseButtonPressed &mP) {
    if (mP.button != sf::Mouse::Button::Left) return;
    handleMouseClick(sf::Vector2i(mP.position.x, mP.position.y));
}

void Game::handleMouseClick(const sf::Vector2i &mousePos) {
    auto *ball = dynamic_cast<Ball *>(m_objects[0].get());
    if (!ball) return;

    sf::Vector2f mouseWorld(static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y));
    sf::Vector2f dir = mouseWorld - ball->getPosition();

    ball->applyImpulse(Constants::IMPULSE * VectorMath::normalize(dir));
}

void Game::update() {
    float dt = m_clock.restart().asSeconds() * m_timeScale;
    for (auto &object : m_objects) object->update(dt);
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    for (auto &object : m_objects) object->draw(m_window);

    auto *ball = dynamic_cast<Ball *>(m_objects[0].get());
    if (ball) {
        DebugDraw::drawDirectionLine(m_window, ball);
        DebugDraw::drawVelocityLine(m_window, ball);
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
