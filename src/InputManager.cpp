#include "InputManager.hpp"

#include <optional>

InputManager::InputManager(KeyCallback keyCb, MouseCallback mouseCb)
    : m_keyCallback(std::move(keyCb)), m_mouseCallback(std::move(mouseCb)) {}

void InputManager::processEvents(sf::Window& window) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }
        if (auto kP = event->getIf<sf::Event::KeyPressed>()) m_keyCallback(*kP);
        if (auto mP = event->getIf<sf::Event::MouseButtonPressed>())
            m_mouseCallback(*mP);
    }
}
