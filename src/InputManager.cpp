#include "InputManager.hpp"

#include <optional>

InputManager::InputManager(KeyCallback keyCb, MouseCallback mouseCb)
    : m_keyCallback(std::move(keyCb)), m_mouseCallback(std::move(mouseCb)) {}

void InputManager::processEvents(sf::Window& window) const {
  while (const std::optional event = window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      window.close();
      return;
    }
    if (const auto kP = event->getIf<sf::Event::KeyPressed>())
      m_keyCallback(*kP);
    if (const auto mP = event->getIf<sf::Event::MouseButtonPressed>())
      m_mouseCallback(*mP);
  }
}
