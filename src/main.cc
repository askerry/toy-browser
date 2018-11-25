#include <iostream>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include <gflags/gflags.h>

#include "dom.h"
#include "layout.h"
#include "parse/css.h"
#include "parse/html.h"
#include "render/paint.h"
#include "render/text.h"
#include "style.h"
#include "util.h"

DEFINE_string(html_file, "examples/demo.html", "html file to load");
DEFINE_string(css_file, "examples/demo.css", "css file to load");
DEFINE_int32(window_width, 1000, "initial width of window");
DEFINE_int32(window_height, 800, "initial height of window");

void render(int width, int height, const style::StyledNode &sn,
            sf::RenderWindow *window) {
  layout::Dimensions viewport;
  viewport.content.width = width;
  viewport.content.height = height;
  std::unique_ptr<layout::LayoutElement> layout_root =
      layout::layout_tree(sn, viewport);
  paint(*layout_root, viewport.content, window);
}

int windowLoop(const style::StyledNode &sn) {
  sf::RenderWindow *window = new sf::RenderWindow();
  window->create(sf::VideoMode(FLAGS_window_width, FLAGS_window_height),
                 "Toy Browser", sf::Style::Close | sf::Style::Resize);
  window->setPosition(sf::Vector2i(0, 0));
  window->clear(sf::Color::Black);
  render(FLAGS_window_width, FLAGS_window_height, sn, window);
  // Run the main event loop as long as the window is open
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window->close();
          break;

        case sf::Event::KeyPressed:
          logger::debug("keypress: " + std::to_string(event.key.code));
          break;

        case sf::Event::Resized:
          logger::debug("new width: " + std::to_string(event.size.width));
          logger::debug("new height: " + std::to_string(event.size.height));
          window->clear(sf::Color::Black);
          render(event.size.width, event.size.height, sn, window);
          break;

        case sf::Event::TextEntered:
          if (event.text.unicode < 128) {
            logger::debug(
                "ASCII character typed: " +
                std::to_string(static_cast<char>(event.text.unicode)));
          }
          break;

        default:
          break;
      }
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  const std::string source = io::readFile(FLAGS_html_file);
  dom::Node *root = html_parser::parseHtml(source);
  const std::string css = io::readFile(FLAGS_css_file);
  const std::unique_ptr<css::StyleSheet const> stylesheet = css::parseCss(css);
  style::PropertyMap pm;
  text_render::FontRegistry *registry =
      text_render::FontRegistry::getInstance();
  std::unique_ptr<style::StyledNode> sn =
      style::styleTree(root, stylesheet, pm);
  windowLoop(*sn);
  registry->clear();
  return 0;
}
