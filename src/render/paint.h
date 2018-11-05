#ifndef PAINT_H
#define PAINT_H

#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "../color.h"
#include "../layout.h"
#include "../parse/css.h"

class RenderCommand {
 protected:
  std::string command_type_;
  layout::Rect rect_;

 public:
  RenderCommand(std::string command_type, layout::Rect rect) {
    command_type_ = command_type;
    rect_ = rect;
  }
  std::string coords();
  void virtual paint(sf::RenderWindow* window){};
};

class RenderShape : public RenderCommand {
  int border_radius_;
  sf::Color color_;

 public:
  RenderShape(std::string command_type, layout::Rect rect, sf::Color color,
              int border_radius)
      : RenderCommand(command_type, rect) {
    color_ = color;
    border_radius_ = border_radius;
  };
  ~RenderShape();
  void paint(sf::RenderWindow* window);
  void log();
};

class RenderText : public RenderCommand {
  std::unique_ptr<sf::Text> text_node_;
  std::string raw_text_;
  sf::Color color_;

 public:
  RenderText(std::string command_type, layout::Rect rect, sf::Color color,
             std::unique_ptr<sf::Text> text_node, std::string raw_text)
      : RenderCommand(command_type, rect) {
    color_ = color;
    text_node_ = std::move(text_node);
    raw_text_ = raw_text;
  };
  std::unique_ptr<sf::Text> take_text_node() { return std::move(text_node_); };
  ~RenderText();
  void paint(sf::RenderWindow* window);
  void log();
};

class RenderImage : public RenderCommand {
  std::string src_;

 public:
  RenderImage(std::string command_type, layout::Rect rect, std::string src)
      : RenderCommand(command_type, rect) {
    src_ = src;
  };
  ~RenderImage();
  void paint(sf::RenderWindow* window);
  void log();
};

class Renderer {
  void renderShape(const layout::LayoutElement&, sf::RenderWindow* window);
  void renderImage(const layout::LayoutElement&, sf::RenderWindow* window);
  void renderBullet(const layout::LayoutElement&, sf::RenderWindow* window);
  void renderText(layout::LayoutElement&, sf::RenderWindow* window);

 public:
  void renderLayout(layout::LayoutElement&, sf::RenderWindow* window);
};

void paint(layout::LayoutElement& layoutRoot, layout::Rect bounds,
           sf::RenderWindow* window);

#endif
