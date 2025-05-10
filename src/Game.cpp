#include "Game.hpp"

#include <SFML/Window/Event.hpp>
#include <cmath>

#include "Constants.hpp"
#include "VectorMath.hpp"

Game::Game() {
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

void Game::processKeyPressed(const sf::Event::KeyPressed& kP) {
    if (kP.code == sf::Keyboard::Key::Equal) {
        m_timeScale *= 1.1f;
    } else if (kP.code == sf::Keyboard::Key::Hyphen) {
        m_timeScale /= 1.1f;
        if (m_timeScale < 0.1f) m_timeScale = 0.1f;
    }
}

void Game::processMousePressed(const sf::Event::MouseButtonPressed& mP) {
    if (mP.button != sf::Mouse::Button::Left) return;
    handleMouseClick(sf::Vector2i(mP.position.x, mP.position.y));
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            return;
        }
        if (auto kP = event->getIf<sf::Event::KeyPressed>())
            processKeyPressed(*kP);
        if (auto mP = event->getIf<sf::Event::MouseButtonPressed>())
            processMousePressed(*mP);
    }
}

void Game::handleMouseClick(const sf::Vector2i& mousePos) {
    auto* ball = dynamic_cast<Ball*>(m_objects[0].get());
    if (!ball) return;

    sf::Vector2f mouseWorld(static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y));
    sf::Vector2f dir = mouseWorld - ball->getPosition();

    ball->applyImpulse(Constants::IMPULSE * VectorMath::normalize(dir));
}

void Game::update() {
    float dt = m_clock.restart().asSeconds() * m_timeScale;
    for (auto& object : m_objects) object->update(dt);
}

void Game::drawLine(const sf::Vector2f& start, const sf::Vector2f& direction,
                    float length, const sf::Color& color) {
    float dirLength =
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f dirNorm =
        (dirLength != 0.f) ? direction / dirLength : sf::Vector2f(1.f, 0.f);
    sf::Vector2f endPoint = start + dirNorm * length;

    sf::Vertex line[] = {{start, color}, {endPoint, color}};
    m_window.draw(line, 2, sf::PrimitiveType::Lines);
}

void Game::drawDirectionLine(const Ball* ball) {
    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2i mousePixel = sf::Mouse::getPosition(m_window);
    sf::Vector2f mouseWorld(static_cast<float>(mousePixel.x),
                            static_cast<float>(mousePixel.y));
    sf::Vector2f dir = mouseWorld - ballCenter;
    drawLine(ballCenter, dir, 100.f, sf::Color::Green);
}

void Game::drawVelocityLine(const Ball* ball) {
    if (ball->isAtRest()) return;
    sf::Vector2f ballCenter = ball->getPosition();
    sf::Vector2f velocity = ball->getVelocity();
    float velLength =
        std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float clampedLength = std::min(100.f, std::max(0.f, velLength));
    drawLine(ballCenter, velocity, clampedLength, sf::Color::Blue);
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    for (auto& object : m_objects) object->draw(m_window);

    auto* ball = dynamic_cast<Ball*>(m_objects[0].get());
    if (ball) {
        drawDirectionLine(ball);
        drawVelocityLine(ball);
    }

    m_window.display();
}

void Game::run() {
    while (m_window.isOpen()) {
        processEvents();
        update();
        render();
    }
}
