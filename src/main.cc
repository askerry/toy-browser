// Main entry point to browser window

#include <iostream>

#include <gflags/gflags.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "dom.h"
#include "layout.h"
#include "parse/css.h"
#include "parse/html.h"
#include "render/paint.h"
#include "render/text.h"
#include "style.h"
#include "util.h"

DEFINE_string(html_file, "examples/demo.html", "HTML file to load");
DEFINE_string(css_file, "examples/demo.css", "CSS file to load");
DEFINE_int32(window_width, 1000, "initial width of window");
DEFINE_int32(window_height, 800, "initial height of window");

namespace {

void renderWindow(int width, int height, const style::StyledNode &sn,
                  sf::RenderWindow *window) {
  layout::Dimensions viewport;
  viewport.content.width = width;
  viewport.content.height = height;
  // Create layout tree for the specified viewport dimensions.
  std::unique_ptr<layout::LayoutElement> layout_root =
      layout::layout_tree(sn, viewport);
  // Paint to window.
  paint(*layout_root, viewport.content, window);
}

int windowLoop(const style::StyledNode &sn) {
  // Create browser window.
  std::unique_ptr<sf::RenderWindow> window(new sf::RenderWindow());
  window->create(sf::VideoMode(FLAGS_window_width, FLAGS_window_height),
                 "Toy Browser", sf::Style::Close | sf::Style::Resize);
  window->setPosition(sf::Vector2i(0, 0));
  window->clear(sf::Color::Black);
  // Render initial window contents.
  renderWindow(FLAGS_window_width, FLAGS_window_height, sn, window.get());
  // Run the main event loop as long as the window is open.
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
          renderWindow(event.size.width, event.size.height, sn, window.get());
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
}  // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // Parse HTML and CSS files.
  const std::string source = io::readFile(FLAGS_html_file);
  std::unique_ptr<dom::Node> root = html_parser::parseHtml(source);
  const std::string css = io::readFile(FLAGS_css_file);
  const std::unique_ptr<css::StyleSheet const> stylesheet = css::parseCss(css);

  // Initialize font registry singleton.
  text_render::FontRegistry *registry =
      text_render::FontRegistry::getInstance();

  // Align styles with DOM nodes.
  std::unique_ptr<style::StyledNode> styled_node =
      style::styleTree(*root, stylesheet, style::PropertyMap());

  // Run main browser window loop.
  windowLoop(*styled_node);

  // Delete styled node and clear font registry.
  styled_node.reset();
  registry->clear();
  return 0;
}
