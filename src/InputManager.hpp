#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <functional>

class InputManager {
   public:
    using KeyCallback = std::function<void(const sf::Event::KeyPressed&)>;
    using MouseCallback =
        std::function<void(const sf::Event::MouseButtonPressed&)>;

    InputManager(KeyCallback keyCb, MouseCallback mouseCb);

    void processEvents(sf::Window& window);

   private:
    KeyCallback m_keyCallback;
    MouseCallback m_mouseCallback;
};
