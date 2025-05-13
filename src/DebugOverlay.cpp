#include "DebugOverlay.hpp"

#include <sstream>

DebugOverlay::DebugOverlay(const std::string &fontPath)
    : m_font(fontPath), m_text(m_font) {
  m_text.setCharacterSize(18);
  m_text.setFillColor(sf::Color::White);
  m_text.setPosition(sf::Vector2f(5.f, 5.f));
}

void DebugOverlay::update(const int drawCalls, const float timeScale,
                          const sf::RenderWindow &window,
                          const size_t threadCount,
                          const size_t ballsPerThread) {
  const float elapsed = m_fpsClock.restart().asSeconds();
  if (elapsed > 0.2f) m_fps = 1.f / elapsed;

  const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

  std::ostringstream oss;
  oss << "Draw calls: " << drawCalls << "\n";
  oss << "Framerate: " << m_fps << " FPS\n";
  oss << "Frame time: " << (elapsed * 1000.f) << " ms\n";
  oss << "Mouse: " << mousePos.x << ", " << mousePos.y << "\n";
  oss << "Time scale: " << timeScale << "\n";
  oss << "\nThreads: " << threadCount;
  oss << "\nBalls per thread: " << ballsPerThread;
  m_text.setString(oss.str());
}

void DebugOverlay::draw(sf::RenderWindow &window) const { window.draw(m_text); }
