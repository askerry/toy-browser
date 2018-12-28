/*
 * Joins a parsed DOM tree and a parsed CSS stylesheet into a single StyledNode
 * tree with styles matched based on selectors.
 */
#ifndef STYLE_H
#define STYLE_H

#include "constants.h"
#include "dom.h"
#include "parse/css.h"

namespace style {

typedef std::map<std::string, std::string> PropertyMap;

typedef std::pair<css::Specificity, css::Rule> MatchedRule;

enum DisplayType {
  Text,
  Inline,
  Block,
  InlineBlock,
  Flex,
  FlexChild,
  Invisible
};

// Represents a DOM node paired with the styles that apply to it.
// Cascading of styles is applied
class StyledNode {
  dom::Node &node_;
  PropertyMap style_values_;
  std::vector<std::unique_ptr<StyledNode>> children_;

 public:
  StyledNode(dom::Node &node, PropertyMap style_values,
             std::vector<std::unique_ptr<StyledNode>> children)
      : node_(node),
        style_values_(style_values),
        children_(std::move(children)){};
  dom::Node &get_node() const { return node_; }
  std::vector<std::reference_wrapper<StyledNode>> get_children() const {
    std::vector<std::reference_wrapper<StyledNode>> children;
    for (auto const &child : children_) {
      children.push_back(*child);
    }
    return children;
  };
  void log() const;
  DisplayType get_display_type() const;
  std::string get_tag() const;
  PropertyMap get_style_values() const { return style_values_; };
};

std::string getValue(const PropertyMap style_value, const std::string &property,
                     const std::string &default_value = constants::DEFAULT);

std::unique_ptr<StyledNode> styleTree(
    dom::Node &root, const std::unique_ptr<css::StyleSheet const> &css,
    PropertyMap parentStyles);
}  // namespace style
#endif
