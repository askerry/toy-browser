#include "text.h"

#include <iostream>

#include "../color.h"
#include "../constants.h"
#include "../util.h"

const std::string DEFAULT_FONT = "14";
const std::string DEFAULT_TEXT_COLOR = "#000000";
const std::string DEFAULT_STYLE = "normal";
const float DEFAULT_LINE_HEIGHT = 1.2;

sf::Color getTextColor(layout::LayoutElement *element) {
  std::string c = element->getStyleValue(constants::css_properties::COLOR,
                                         DEFAULT_TEXT_COLOR);
  sf::Color color = parseColor(c);
  return color;
}

int getSize(layout::LayoutElement *element) {
  return std::stoi(element->getStyleValue(constants::css_properties::FONT_SIZE,
                                          DEFAULT_FONT));
}

sf::Uint32 getStyle(layout::LayoutElement *element) {
  std::string style = element->getStyleValue(
      constants::css_properties::FONT_STYLE, DEFAULT_STYLE);
  std::string weight = element->getStyleValue(
      constants::css_properties::FONT_WEIGHT, DEFAULT_STYLE);
  std::string decoration = element->getStyleValue(
      constants::css_properties::TEXT_DECORATION, DEFAULT_STYLE);
  sf::Uint32 textStyle = sf::Text::Regular;
  if (weight == constants::css_font_values::BOLD) {
    textStyle = textStyle | sf::Text::Bold;
  }
  if (style == constants::css_font_values::ITALIC) {
    textStyle = textStyle | sf::Text::Italic;
  }
  if (decoration == constants::css_font_values::UNDERLINE) {
    textStyle = textStyle | sf::Text::Underlined;
  }
  return textStyle;
}

std::unique_ptr<sf::Font> loadFont(const std::string &fontName) {
  std::unique_ptr<sf::Font> font(new sf::Font);
  if (!font->loadFromFile("fonts/" + fontName + ".ttf")) {
    logger::error("Unable to load font: " + fontName);
  }
  return font;
}

namespace text_render {

int getTextWidth(layout::LayoutElement *element) {
  sf::FloatRect rect = element->get_text_node().getGlobalBounds();
  return rect.width + rect.left;
}

int getTextHeight(layout::LayoutElement *element) {
  int fontSize = getSize(element);
  std::string lineHeight =
      element->getStyleValue(constants::css_properties::LINE_HEIGHT);
  if (lineHeight == constants::DEFAULT) {
    return fontSize * DEFAULT_LINE_HEIGHT;
  } else {
    return std::stoi(lineHeight);
  }
}

std::unique_ptr<sf::Text> constructText(layout::LayoutElement *element,
                                        const std::string &rawText) {
  std::unique_ptr<sf::Text> text(new sf::Text);
  text->setFont(FontRegistry::getInstance()->load("Arial"));
  text->setString(rawText);
  text->setCharacterSize(getSize(element));
  text->setFillColor(getTextColor(element));
  text->setStyle(getStyle(element));
  return text;
}

// Global static pointer to font registry
FontRegistry *FontRegistry::instance_ = NULL;

const sf::Font &FontRegistry::load(std::string fontName) {
  if (fonts_.find(fontName) == fonts_.end()) {
    fonts_[fontName] = loadFont(fontName);
  }
  return *fonts_[fontName];
}

FontRegistry *FontRegistry::getInstance() {
  if (!instance_) {
    instance_ = new FontRegistry;
  }
  return instance_;
}
void FontRegistry::clear() {
  logger::info("Clearing font registry");
  for (auto const &it : fonts_) {
    // delete it.second;
  }
  fonts_.clear();
}
}  // namespace text_render
