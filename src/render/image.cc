#include "image.h"

std::pair<float, float> getScalars(sf::Sprite sprite, int width, int height,
                                   std::string style = "default") {
  float xScale = 1;
  float yScale = 1;
  sf::FloatRect r = sprite.getGlobalBounds();
  if (height > 0 && width > 0) {
    xScale = (float)width / r.width;
    yScale = (float)height / r.height;
  } else if (width > 0) {
    xScale = (float)width / r.width;
    yScale = xScale;

  } else if (height > 0) {
    yScale = (float)height / r.height;
    xScale = yScale;
  }
  return std::pair<float, float>(xScale, yScale);
}

namespace image_render {
void drawImage(sf::RenderWindow* window, const std::string& imageFile, int x,
               int y, int width, int height) {
  sf::Texture texture;
  if (!texture.loadFromFile("examples/" + imageFile)) {
    logger::error("Failed to load image: examples/" + imageFile);
  }
  sf::Sprite sprite;
  sprite.setTexture(texture, true);
  std::pair<float, float> scalars = getScalars(sprite, width, height);
  sprite.setPosition(sf::Vector2f(x, y));
  sprite.setScale(sf::Vector2f(scalars.first, scalars.second));
  window->draw(sprite);
}
}  // namespace image_render
