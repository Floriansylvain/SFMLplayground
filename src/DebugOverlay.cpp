#include "DebugOverlay.hpp"

#include <iomanip>
#include <sstream>

namespace {

constexpr float OVERLAY_MARGIN = 5.0f;
constexpr unsigned int FONT_SIZE = 18;
constexpr float AVG_UPDATE_INTERVAL = 0.5f;
constexpr float TEXT_UPDATE_INTERVAL = 0.05f;
}  // namespace

DebugOverlay::DebugOverlay(const std::string &fontPath)
    : m_font(fontPath), m_text(m_font) {
  m_text.setCharacterSize(FONT_SIZE);
  m_text.setFillColor(sf::Color::White);
  m_text.setPosition({OVERLAY_MARGIN, OVERLAY_MARGIN});
  m_frameTimeHistory.fill(0.0f);
}

void DebugOverlay::update(const int drawCalls, const float timeScale,
                          const sf::RenderWindow &window,
                          const size_t threadCount, const size_t totalBalls,
                          const size_t ballsPerThread) {
  const float frameTime = m_fpsClock.restart().asSeconds();

  m_frameTimeHistory[m_currentFrameIndex] = frameTime;
  m_currentFrameIndex = (m_currentFrameIndex + 1) % FRAME_HISTORY_SIZE;

  m_timeSinceLastAvgUpdate += frameTime;
  if (m_timeSinceLastAvgUpdate >= AVG_UPDATE_INTERVAL) {
    m_averageFrametime = 0.0f;
    for (const float time : m_frameTimeHistory) m_averageFrametime += time;
    m_averageFrametime /= FRAME_HISTORY_SIZE;
    m_timeSinceLastAvgUpdate = 0.0f;
  }

  m_timeSinceLastUpdate += frameTime;
  if (m_timeSinceLastUpdate < TEXT_UPDATE_INTERVAL) return;
  m_timeSinceLastUpdate = 0.0f;

  const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

  std::ostringstream oss;

  oss << "Draw calls: " << drawCalls << "\n";

  oss << std::fixed << std::setprecision(3);
  oss << "Frame time: " << (frameTime * 1000.0f) << " ms\n";
  oss << "Frame time (avg): " << (m_averageFrametime * 1000.0f) << " ms\n";

  oss << std::defaultfloat << std::setprecision(6);
  oss << "Mouse: " << mousePos.x << ", " << mousePos.y << "\n";
  oss << "Time scale: " << timeScale << "\n";
  oss << "\nThreads: " << threadCount;
  oss << "\nBalls per thread: " << ballsPerThread << "\n";

  oss << "\nTotal Balls: " << totalBalls;

  m_text.setString(oss.str());
}

void DebugOverlay::draw(sf::RenderWindow &window) const { window.draw(m_text); }