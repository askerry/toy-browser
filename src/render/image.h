#ifndef IMAGE_H
#define IMAGE_H

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "../util.h"

namespace image_render {

void drawImage(sf::RenderWindow* window, const std::string& imageFile,
               int x = 0, int y = 0, int width = -1, int height = -1);
}

#endif
