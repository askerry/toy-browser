/*
 * Converts a StyledNode tree into a tree of LayoutElements capturing how
 * elements will be laid out on the page at a pixel level.
 */
#ifndef LAYOUT_H
#define LAYOUT_H

#include "SFML/Graphics.hpp"

#include "constants.h"
#include "style.h"

namespace layout {

struct Rect {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
};

struct EdgeSizes {
  int left = 0;
  int right = 0;
  int top = 0;
  int bottom = 0;
};

struct Dimensions {
 public:
  // Position of the content area relative to the document origin:
  Rect content;

  // Surrounding edges:
  EdgeSizes padding;
  EdgeSizes border;
  EdgeSizes margin;

  Rect paddingBox() const;
  Rect borderBox() const;
  Rect marginBox() const;
};

enum BoxType { Img, Text, Bullet, Shape };

class LayoutElement {
  std::vector<std::unique_ptr<LayoutElement>> children_;
  std::string raw_data_;
  std::unique_ptr<sf::Text> text_node_;
  style::PropertyMap style_values_;
  BoxType box_type_;
  style::DisplayType display_type_;
  void calculateWidth(Dimensions container);
  void calculatePosition(Dimensions container, int xCursor, int yCursor,
                         bool shouldRenderBelow);
  void setHeight();
  void layoutChildren(int parentWidth);

 public:
  Dimensions dimensions;
  LayoutElement(dom::Node *node, style::PropertyMap style_values,
                style::DisplayType display_type, BoxType box_type);
  std::string get_raw_data() const { return raw_data_; };
  BoxType get_box_type() const { return box_type_; };
  style::DisplayType get_display_type() const { return display_type_; };
  std::unique_ptr<sf::Text> take_text_node() { return std::move(text_node_); };
  const sf::Text &get_text_node() { return *text_node_; };
  std::vector<std::reference_wrapper<LayoutElement>> get_children() const {
    std::vector<std::reference_wrapper<LayoutElement>> children;
    for (auto const &child : children_) {
      children.push_back(*child);
    }
    return children;
  };
  void addChild(std::unique_ptr<LayoutElement> child) {
    children_.push_back(std::move(child));
  }
  void applyLayout(Dimensions container, int xCursor = 0, int yCursor = 0,
                   bool shouldRenderBelow = true);
  std::string getStyleValue(
      const std::string &property,
      const std::string &defaultValue = constants::DEFAULT) const {
    return style::getValue(style_values_, property, defaultValue);
  }
};

std::unique_ptr<LayoutElement> build_layout_tree(
    const style::StyledNode &styleTree);

std::unique_ptr<LayoutElement> layout_tree(const style::StyledNode &styleTree,
                                           Dimensions container);
}  // namespace layout

#endif
