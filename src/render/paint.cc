#include "paint.h"

#include "../util.h"
#include "image.h"
#include "shape.h"

RenderShape::~RenderShape() {
  std::cout << "Destructing render shape" << std::endl;
  // delete color_;
}

RenderImage::~RenderImage() {
  std::cout << "Destructing render image" << std::endl;
}

RenderText::~RenderText() {
  std::cout << "Destructing render text" << std::endl;
  // delete text_node_;
}

void Renderer::renderLayout(layout::LayoutElement &box,
                            sf::RenderWindow *window) {
  if (box.get_display_type() == style::Invisible) {
    return;
  } else if (box.get_box_type() == layout::Img) {
    this->renderImage(box, window);
  } else if (box.get_box_type() == layout::Text) {
    this->renderText(box, window);
  } else if (box.get_box_type() == layout::Bullet) {
    this->renderBullet(box, window);
  } else {
    this->renderShape(box, window);
  }
  for (auto child : box.get_children()) {
    renderLayout(child, window);
  }
}
void Renderer::renderBullet(const layout::LayoutElement &box,
                            sf::RenderWindow *window) {
  sf::Color color = getColor(box, constants::css_properties::COLOR);
  layout::Rect r = box.dimensions.paddingBox();
  layout::Rect bullet_rect;
  bullet_rect.x = r.x;
  bullet_rect.y = r.y + r.height / 2;
  bullet_rect.height = 5;
  bullet_rect.width = 5;
  RenderShape command = RenderShape("Bullet", bullet_rect, color, 0);
  command.paint(window);
}
void Renderer::renderShape(const layout::LayoutElement &box,
                           sf::RenderWindow *window) {
  int borderRadius = std::stoi(
      box.getStyleValue(constants::css_properties::BORDER_RADIUS, "0"));
  sf::Color b_color = getColor(box, constants::css_properties::BORDER_COLOR);
  RenderShape command1("Border", box.dimensions.borderBox(), b_color,
                       borderRadius);
  command1.paint(window);
  command1.log();

  sf::Color bg_color =
      getColor(box, constants::css_properties::BACKGROUND_COLOR);
  RenderShape command("Rect", box.dimensions.paddingBox(), bg_color,
                      borderRadius);
  command.paint(window);
}
void Renderer::renderImage(const layout::LayoutElement &box,
                           sf::RenderWindow *window) {
  RenderImage command("Image", box.dimensions.borderBox(), box.get_raw_data());
  command.paint(window);
}
void Renderer::renderText(layout::LayoutElement &box,
                          sf::RenderWindow *window) {
  sf::Color color = getColor(box, constants::css_properties::BACKGROUND_COLOR);
  RenderText command("Text", box.dimensions.borderBox(), color,
                     box.take_text_node(), box.get_raw_data());
  command.paint(window);
}

void RenderShape::paint(sf::RenderWindow *window) {
  sf::Vector2u size = window->getSize();
  int windowWidth = size.x;
  int windowHeight = size.y;
  int x0 = std::max(0, rect_.x);
  int x1 = std::min(windowWidth, rect_.x + rect_.width);
  int y0 = std::max(0, rect_.y);
  int y1 = std::min(windowHeight, rect_.y + rect_.height);
  shape_render::drawRect(window, x0, y0, x1, y1, color_, border_radius_);
  log();
}

void RenderText::paint(sf::RenderWindow *window) {
  int x0 = std::max(0, rect_.x);
  int y0 = std::max(0, rect_.y);
  std::unique_ptr<sf::Text> text = take_text_node();
  text->setPosition(x0, y0);
  window->draw(*text);
  log();
}

void RenderImage::paint(sf::RenderWindow *window) {
  image_render::drawImage(window, src_, rect_.x, rect_.y, rect_.width,
                          rect_.height);
  log();
}

std::string RenderCommand::coords() {
  std::string str = "... x=" + std::to_string(rect_.x) +
                    ", y=" + std::to_string(rect_.y) + ", width=";
  str += std::to_string(rect_.width) +
         ", height=" + std::to_string(rect_.height) + ", ";
  return str;
}

void RenderText::log() {
  std::string str = "Command= " + command_type_ + ": ";
  str += "'" + raw_text_ + "'";
  str += RenderCommand::coords();
  str += displayColor(color_);
  logger::info(str);
}

void RenderImage::log() {
  std::string str = "Command= " + command_type_ + ": ";
  str += "src='" + src_ + "'";
  str += RenderCommand::coords();
  logger::info(str);
}

void RenderShape::log() {
  std::string str = "Command= " + command_type_ + ": ";
  str += RenderCommand::coords();
  str += displayColor(color_);
  logger::info(str);
}

void paint(layout::LayoutElement &layoutRoot, layout::Rect bounds,
           sf::RenderWindow *window) {
  logger::info("\n****** Painting canvas ******");
  Renderer Renderer;
  Renderer.renderLayout(layoutRoot, window);
  window->display();
}
