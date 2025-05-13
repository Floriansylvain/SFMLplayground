#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

class DebugOverlay {
 public:
  explicit DebugOverlay(const std::string &fontPath);

  void update(int drawCalls, float timeScale, const sf::RenderWindow &window,
              size_t threadCount, size_t ballsPerThread);

  void draw(sf::RenderWindow &window) const;

 private:
  sf::Font m_font;
  sf::Text m_text;
  sf::Clock m_fpsClock;
  float m_fps = 0.f;
};
