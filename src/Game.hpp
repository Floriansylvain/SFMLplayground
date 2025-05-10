#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <vector>

#include "Ball.hpp"
#include "InputManager.hpp"
#include "PhysicalObject.hpp"

class Ball;
class PhysicalObject;

class Game {
   private:
    float m_timeScale;
    sf::RenderWindow m_window;
    std::vector<std::unique_ptr<PhysicalObject>> m_objects;
    sf::Clock m_clock;
    InputManager m_inputManager;
    sf::VertexArray m_debugLines;
    void processKeyPressed(const sf::Event::KeyPressed& keyPressed);
    void processMousePressed(const sf::Event::MouseButtonPressed& mousePressed);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void update();
    void render();

   public:
    Game();
    void run();
};
