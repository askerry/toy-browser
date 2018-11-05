#include "shape.h"

#include <cmath>

sf::ConvexShape RoundedRectangle(float x, float y, float rectWidth,
                                 float rectHeight, float radius,
                                 int points = 10) {
  sf::ConvexShape rrect;
  rrect.setPointCount(points * 4);
  float X = 0, Y = 0;
  for (int i = 0; i < points; i++) {
    X += radius / points;
    Y = sqrt(radius * radius - X * X);
    sf::Vector2f v(X + x + rectWidth - radius, y - Y + radius);
    rrect.setPoint(i, v);
  }
  Y = 0;
  for (int i = 0; i < points; i++) {
    Y += radius / points;
    X = sqrt(radius * radius - Y * Y);
    rrect.setPoint(points + i, sf::Vector2f(x + rectWidth + X - radius,
                                            y + rectHeight - radius + Y));
  }
  X = 0;
  for (int i = 0; i < points; i++) {
    X += radius / points;
    Y = sqrt(radius * radius - X * X);
    rrect.setPoint(points * 2 + i,
                   sf::Vector2f(x + radius - X, y + rectHeight - radius + Y));
  }
  Y = 0;
  for (int i = 0; i < points; i++) {
    Y += radius / points;
    X = sqrt(radius * radius - Y * Y);
    rrect.setPoint(points * 3 + i,
                   sf::Vector2f(x - X + radius, y + radius - Y));
  }
  return rrect;
}

namespace shape_render {

void drawRect(sf::RenderWindow* window, int x0, int y0, int x1, int y1,
              sf::Color c, int borderRadius) {
  float width = x1 - x0;
  float height = y1 - y0;
  if (borderRadius > 0) {
    sf::ConvexShape shape = RoundedRectangle(x0, y0, width, height, 10);
    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    window->draw(shape);
  } else {
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(width, height));
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));
    rectangle.setPosition(x0, y0);
    window->draw(rectangle);
  }
}
}  // namespace shape_render
