#include "DebugOverlay.hpp"

#include <sstream>

DebugOverlay::DebugOverlay(const std::string& fontPath)
    : m_font(fontPath), m_text(m_font) {
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(sf::Vector2f(5.f, 5.f));
}

void DebugOverlay::update(int drawCalls, float timeScale,
                          sf::RenderWindow& window, size_t threadCount,
                          size_t ballsPerThread) {
    float elapsed = m_fpsClock.restart().asSeconds();
    if (elapsed > 0.f) m_fps = static_cast<int>(1.f / elapsed);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    std::ostringstream oss;
    oss << "Draw calls: " << drawCalls << "\n";
    oss << "Framerate: " << m_fps << " FPS\n";
    oss << "Frametime: " << (elapsed * 1000.f) << " ms\n";
    oss << "Mouse: " << mousePos.x << ", " << mousePos.y << "\n";
    oss << "Time scale: " << timeScale << "\n";
    oss << "\nThreads: " << threadCount;
    oss << "\nBalls per thread: " << ballsPerThread;
    m_text.setString(oss.str());
}

void DebugOverlay::draw(sf::RenderWindow& window) { window.draw(m_text); }
