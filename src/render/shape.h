#ifndef SHAPE_H
#define SHAPE_H

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

namespace shape_render {

void drawRect(sf::RenderWindow* window, int x0, int y0, int x1, int y1,
              sf::Color c, int borderRadius = 0);
}

#endif
