#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "../layout.h"

namespace text_render {
class FontRegistry {
  std::map<std::string, std::unique_ptr<sf::Font>> fonts_;
  static FontRegistry* instance_;
  // constuctors and assignment operations private so they can't be called
  FontRegistry() {}
  FontRegistry(FontRegistry const&){};
  FontRegistry& operator=(FontRegistry const&);

 public:
  const sf::Font& load(std::string fontName);
  static FontRegistry* getInstance();
  void clear();
};
int getTextHeight(layout::LayoutElement* element);
int getTextWidth(layout::LayoutElement* element);
std::unique_ptr<sf::Text> constructText(layout::LayoutElement* element,
                                        const std::string& rawText);
}  // namespace text_render

#endif
