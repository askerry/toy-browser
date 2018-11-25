#include "style.h"

#include <iostream>

#include "absl/strings/match.h"
#include "absl/strings/str_split.h"

#include "util.h"

const std::vector<std::string> INLINE_TAGS = {
    constants::html_tags::EM,   constants::html_tags::ITALIC,
    constants::html_tags::SPAN, constants::html_tags::A,
    constants::html_tags::BOLD, constants::html_tags::IMG};
const std::vector<std::string> EXPANDABLE_PROPERTIES = {
    constants::css_properties::MARGIN, constants::css_properties::PADDING};
const std::vector<std::string> BORDER_PROPERTIES = {
    constants::css_properties::BORDER_WIDTH,
    constants::css_properties::BORDER_COLOR,
    constants::css_properties::BORDER_STYLE};
const std::vector<std::string> INHERITABLE_STYLES = {
    constants::css_properties::COLOR,
    constants::css_properties::BACKGROUND_COLOR,
    constants::css_properties::FONT_SIZE,
    constants::css_properties::FONT_FAMILY,
    constants::css_properties::FONT_WEIGHT,
    constants::css_properties::FONT_STYLE,
    constants::css_properties::LINE_HEIGHT,
    constants::css_properties::TEXT_DECORATION,
};

// Determine if the tag is an inline or block element by default
std::string getDefaultDisplay(const std::string &tag_name) {
  if (tag_name == constants::css_display_types::TEXT) {
    return constants::css_display_types::TEXT;
  } else if (std::find(INLINE_TAGS.begin(), INLINE_TAGS.end(), tag_name) !=
             INLINE_TAGS.end()) {
    return constants::css_display_types::INLINE;
  } else {
    return constants::css_display_types::BLOCK;
  }
}

namespace style {
typedef std::pair<MatchedRule, bool> MaybeMatchedRule;

DisplayType StyledNode::get_display_type() const {
  std::string displayStr =
      getValue(style_values_, constants::css_properties::DISPLAY,
               getDefaultDisplay(get_tag()));
  if (displayStr == constants::css_display_types::TEXT) {
    return Text;
  } else if (displayStr == constants::css_display_types::INLINE) {
    return Inline;
  } else if (displayStr == constants::css_display_types::INLINE_BLOCK) {
    return InlineBlock;
  } else if (displayStr == constants::css_display_types::BLOCK) {
    return Block;
  } else if (displayStr == constants::css_display_types::FLEX) {
    return Flex;
  } else if (displayStr == constants::css_display_types::FLEX_CHILD) {
    return FlexChild;
  } else if (displayStr == constants::css_display_types::NONE) {
    return Invisible;
  } else {
    return Block;
  }
}

std::string StyledNode::get_tag() const {
  dom::Node &node = get_node();
  try {
    dom::ElementNode &castToElement = dynamic_cast<dom::ElementNode &>(node);
    return castToElement.get_tag();
  } catch (const std::bad_cast &e) {
    return constants::html_tags::TEXT;
  }
}

void StyledNode::log() const {
  std::string log_str = "\n Node: " + get_node().toLogStr() + "\n Styles: \n";
  for (auto it = style_values_.cbegin(); it != style_values_.cend(); ++it) {
    log_str += "\t" + it->first + ": " + it->second + "\n";
  }
  logger::info(log_str);
}

// Return the specified style property.
std::string getValue(const PropertyMap style_values,
                     const std::string &property,
                     const std::string &default_value) {
  if (style_values.find(property) == style_values.end()) {
    return default_value;
  }
  if (style_values.at(property) == "auto") {
    return "-1";
  }
  return style_values.at(property);
}

// Returns true of the node matches the specified selector.
bool isMatch(dom::ElementNode *node, css::Selector selector) {
  if (selector.get_tag() == "*") {
    return true;
  } else if (node->get_tag() == selector.get_tag()) {
    return true;
  } else if (!node->get_id().empty() && (node->get_id() == selector.get_id())) {
    return true;
  } else {
    std::vector<std::string> node_classes = node->get_classes();
    for (auto s : selector.get_classes()) {
      if (std::find(node_classes.begin(), node_classes.end(), s) !=
          node_classes.end()) {
        return true;
      }
    }
    return false;
  }
}

// If `rule` matches `elem`, return a `MatchedRule`. Otherwise return `None`.
MaybeMatchedRule matchingRule(dom::ElementNode *node, css::Rule rule) {
  std::vector<css::Selector> selectors = rule.get_selectors();
  css::Specificity tup;
  for (auto s : selectors) {
    if (isMatch(node, s)) {
      tup = s.getSpecificity();
      MatchedRule result(tup, rule);
      return std::pair<MatchedRule, bool>(result, true);
    }
  }
  MatchedRule result(tup, rule);
  return std::pair<MatchedRule, bool>(result, false);
}

bool compareRules(MatchedRule r1, MatchedRule r2) {
  css::Specificity s1 = r1.first;
  css::Specificity s2 = r2.first;
  return s1 < s2;
}

std::string parseSizeValues(const std::string &key,
                            const std::string &raw_value, PropertyMap &pm) {
  if (raw_value.size() < 2) {
    return raw_value;
  }
  std::string double_suffix = raw_value.substr(raw_value.size() - 2, 2);
  std::string single_suffix = raw_value.substr(raw_value.size() - 1, 1);
  if (double_suffix == "em") {
    int font_size = std::stoi(pm[constants::css_properties::FONT_SIZE]);
    int number = std::stoi(raw_value.substr(0, raw_value.size() - 2));
    return std::to_string(number * font_size);

  } else if (double_suffix == "px") {
    return raw_value.substr(0, raw_value.size() - 2);
  } else if (single_suffix == "%") {
    logger::error("Percent-based sizing is not supported");
  }
  return raw_value;
}

void parseBorderShorthand(PropertyMap &pm, const std::string &name,
                          const std::string &value) {}

void fillPropertyMap(PropertyMap &pm, PropertyMap &parentStyles,
                     css::Declaration declaration) {
  std::string value = declaration.get_value();
  std::string name = declaration.get_name();
  if (name == constants::css_properties::BORDER) {
    parseBorderShorthand(pm, name, value);
  } else {
    value = parseSizeValues(name, value, parentStyles);
    pm[name] = value;
  }
}

bool shouldInherit(const std::string &cssProperty) {
  return std::find(INHERITABLE_STYLES.begin(), INHERITABLE_STYLES.end(),
                   cssProperty) != INHERITABLE_STYLES.end();
}

PropertyMap getTextStyleValues(PropertyMap parent_styles) {
  PropertyMap styles;
  for (auto const &style : parent_styles) {
    if (shouldInherit(style.first)) {
      styles[style.first] = style.second;
    }
  }
  return styles;
}

void expandStyles(PropertyMap &styles) {
  for (auto const &style : styles) {
    if (std::find(EXPANDABLE_PROPERTIES.begin(), EXPANDABLE_PROPERTIES.end(),
                  style.first) != EXPANDABLE_PROPERTIES.end()) {
      styles[style.first + "-left"] = style.second;
      styles[style.first + "-right"] = style.second;
      styles[style.first + "-top"] = style.second;
      styles[style.first + "-bottom"] = style.second;
    }
    if (std::find(BORDER_PROPERTIES.begin(), BORDER_PROPERTIES.end(),
                  style.first) != BORDER_PROPERTIES.end()) {
      std::vector<std::string> split = absl::StrSplit(style.first, '-');
      styles[split[0] + "-left-" + split[1]] = style.second;
      styles[split[0] + "-right-" + split[1]] = style.second;
      styles[split[0] + "-top-" + split[1]] = style.second;
      styles[split[0] + "-bottom-" + split[1]] = style.second;
    }
  }
}

// Construct full set of styles to apply to this node.
PropertyMap getElementStyleValues(
    dom::ElementNode *node, const std::unique_ptr<css::StyleSheet const> &css,
    PropertyMap parent_styles) {
  PropertyMap styles;
  // Initialize with styles inherited from the parent
  for (auto const &style : parent_styles) {
    if (shouldInherit(style.first)) {
      styles[style.first] = style.second;
    }
    // Propogate parent's flex display by setting "flex-child" on children
    if (style.first == constants::css_properties::DISPLAY &&
        style.second == constants::css_display_types::FLEX) {
      styles[constants::css_properties::DISPLAY] =
          constants::css_display_types::FLEX_CHILD;
    }
    // Propogate parent's text-alignment by setting appropriate margins on child
    // if (style.first == "text-align") {
    //   if (style.second == "center") {
    //     styles["margin-right"] = "auto";
    //     styles["margin-left"] = "auto";
    //   } else if (style.second == "right") {
    //     styles["margin-left"] = "auto";
    //   } else {
    //     styles["margin-right"] = "auto";
    //   }
    // }
  }

  // Find all matching rules
  std::vector<MatchedRule> matching_rules;
  std::vector<css::Rule> all_rules = css->get_rules();
  for (auto rule : all_rules) {
    MaybeMatchedRule m = matchingRule(node, rule);
    if (m.second) {
      matching_rules.push_back(m.first);
    }
  }
  // Go through the rules from lowest to highest specificity.
  std::sort(matching_rules.begin(), matching_rules.end(), compareRules);
  for (auto match : matching_rules) {
    css::Rule rule = match.second;
    for (auto declaration : rule.get_declarations()) {
      fillPropertyMap(styles, parent_styles, declaration);
    }
    // delete match;
  }
  expandStyles(styles);
  return styles;
}

// Construct a tree of StyledNodes from a DOM tree + StyleSheet
std::unique_ptr<StyledNode> styleTree(
    dom::Node &root, const std::unique_ptr<css::StyleSheet const> &css,
    PropertyMap parentStyles) {
  std::unique_ptr<StyledNode> s;
  std::vector<std::unique_ptr<StyledNode>> children;
  PropertyMap styles;
  try {
    // Determine if this is an element node, and recursively apply styles
    dom::ElementNode &castToElement = dynamic_cast<dom::ElementNode &>(root);
    if (!castToElement.isDisplayable()) {
      s.reset(new StyledNode(castToElement, styles, std::move(children)));

    } else {
      styles = getElementStyleValues(&castToElement, css, parentStyles);
      for (auto nodeChild : castToElement.get_children()) {
        std::unique_ptr<StyledNode> styledChild =
            styleTree(nodeChild, css, styles);
        children.push_back(std::move(styledChild));
      }
      s.reset(new StyledNode(castToElement, styles, std::move(children)));
    }
  } catch (const std::bad_cast &e) {
    dom::TextNode &castToText = dynamic_cast<dom::TextNode &>(root);
    styles = getTextStyleValues(parentStyles);
    s.reset(new StyledNode(castToText, styles, std::move(children)));
  }
  s->log();
  return s;
}
}  // namespace style
