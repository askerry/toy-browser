#ifndef COLOR_H
#define COLOR_H

#include <string>
#include "SFML/Graphics.hpp"

#include "layout.h"

namespace color {

sf::Color parseColor(std::string rawColor);

sf::Color getColor(const layout::LayoutElement& box,
                   const std::string& colorType);

inline std::string toLogStr(sf::Color color) {
  return "RGB=" + std::to_string(unsigned(color.r)) + "," +
         std::to_string(unsigned(color.g)) + "," +
         std::to_string(unsigned(color.b)) + "," +
         std::to_string(unsigned(color.a));
}
}  // namespace color

#endif
