#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "../layout.h"

namespace text_render {

// Registry singleton for SFML fonts
class FontRegistry {
  std::map<std::string, std::unique_ptr<sf::Font>> fonts_;
  static FontRegistry* instance_;
  // Make default constuctor private so it can't be called
  FontRegistry() {}

  // Delete copy constructor and copy assignment operator
  FontRegistry(const FontRegistry&) = delete;
  FontRegistry& operator=(const FontRegistry&) = delete;

 public:
  const sf::Font& load(const std::string& fontName);
  static FontRegistry* getInstance();
  void clear();
};

int getTextHeight(layout::LayoutElement* element);
int getTextWidth(layout::LayoutElement* element);
std::unique_ptr<sf::Text> constructText(layout::LayoutElement* element,
                                        const std::string& rawText);
}  // namespace text_render

#endif
