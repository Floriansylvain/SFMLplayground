#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Ball.hpp"
#include "BatchRenderer.hpp"
#include "DebugOverlay.hpp"
#include "InputManager.hpp"
#include "PhysicalObject.hpp"
#include "ThreadPool.hpp"

class Ball;
class PhysicalObject;

class Game {
   private:
    float m_timeScale = 1.f;
    sf::RenderWindow m_window;
    sf::Vector2f m_windowSize;
    std::vector<std::unique_ptr<PhysicalObject>> m_objects;
    sf::Clock m_clock;
    InputManager m_inputManager;
    sf::VertexArray m_debugLines;
    int m_drawCallCount = 0;
    DebugOverlay m_debugOverlay;
    bool m_toggleDebug = true;
    BatchRenderer m_batchRenderer;
    ThreadPool m_threadPool;

    struct CellHash {
        std::size_t operator()(const std::pair<int, int>& k) const {
            return static_cast<std::size_t>(k.first) * 73856093 ^
                   static_cast<std::size_t>(k.second) * 19349663;
        }
    };
    using Cell = std::pair<int, int>;
    using Grid = std::unordered_map<Cell, std::vector<Ball*>, CellHash>;

    void processKeyPressed(const sf::Event::KeyPressed& keyPressed);
    void processMousePressed(const sf::Event::MouseButtonPressed& mousePressed);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void update();
    void render();
    void updateBallsParallel(float dt);
    void resolveSpatialCollisionsParallel(const Grid& grid);

    Grid buildSpatialGrid();

   public:
    Game();
    void run();
    size_t getThreadCount() const { return m_threadPool.getThreadCount(); }
};
