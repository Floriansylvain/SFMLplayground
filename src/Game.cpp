#include "Game.hpp"

#include <SFML/Window/Event.hpp>

#include "Ball.hpp"
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

void Game::processKeyPressed(const sf::Event::KeyPressed& keyPressed) {
    if (keyPressed.code == sf::Keyboard::Key::Add ||
        keyPressed.code == sf::Keyboard::Key::Equal) {
        m_timeScale *= 1.1f;
    } else if (keyPressed.code == sf::Keyboard::Key::Subtract ||
               keyPressed.code == sf::Keyboard::Key::Hyphen) {
        m_timeScale /= 1.1f;
        if (m_timeScale < 0.1f) m_timeScale = 0.1f;
    }
}

void Game::processMousePressed(
    const sf::Event::MouseButtonPressed& mousePressed) {
    if (mousePressed.button == sf::Mouse::Button::Left) {
        handleMouseClick(
            sf::Vector2i(mousePressed.position.x, mousePressed.position.y));
    }
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
    sf::Vector2f ballPos = ball->getPosition();
    sf::Vector2f mouseWorld(static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y));
    sf::Vector2f dir = mouseWorld - ballPos;

    dir = VectorMath::normalize(dir);

    ball->applyImpulse(2000.f * dir);
}

void Game::update() {
    float dt = m_clock.restart().asSeconds() * m_timeScale;
    for (auto& object : m_objects) {
        object->update(dt);
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    for (auto& object : m_objects) {
        object->draw(m_window);
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
