#include "layout.h"

#include "absl/strings/ascii.h"
#include "absl/strings/str_format.h"

#include "render/text.h"
#include "util.h"

namespace layout {
Rect expand(Rect rect, EdgeSizes edge) {
  Rect expanded;
  expanded.x = rect.x - edge.left;
  expanded.y = rect.y - edge.top;
  expanded.width = rect.width + edge.left + edge.right;
  expanded.height = rect.height + edge.top + edge.bottom;
  return expanded;
}

Rect Dimensions::paddingBox() const { return expand(content, padding); }
Rect Dimensions::borderBox() const { return expand(paddingBox(), border); }
Rect Dimensions::marginBox() const { return expand(borderBox(), margin); }

void LayoutElement::setHeight() {
  // If the height is set to an explicit length, use that exact length.
  // Otherwise, we just keep the value set by `layoutChildren`.
  if (get_display_type() == style::Text) {
    dimensions.content.height = text_render::getTextHeight(this);
  } else if (getStyleValue(constants::css_properties::HEIGHT, "-1") != "-1") {
    int height = std::stoi(getStyleValue(constants::css_properties::HEIGHT));
    dimensions.content.height = height;
  }
}

LayoutElement::LayoutElement(dom::Node &node, style::PropertyMap style_values,
                             style::DisplayType display_type,
                             BoxType box_type) {
  display_type_ = display_type;
  box_type_ = box_type;
  style_values_ = style_values;

  if (display_type == style::Text) {
    dom::TextNode &castToText = dynamic_cast<dom::TextNode &>(node);
    if (&castToText != &node) {
      logger::error("The provided node is not a text node");
    }
    raw_data_ = castToText.get_text();
    text_node_ = text_render::constructText(this, castToText.get_text());
  }
  if (box_type == Img) {
    dom::ElementNode &castToElement = dynamic_cast<dom::ElementNode &>(node);
    raw_data_ = castToElement.getAttr(constants::html_attributes::SRC, "/");
  }
}

void LayoutElement::applyLayout(Dimensions container, int xCursor, int yCursor,
                                bool shouldRenderBelow) {
  // Child width can depend on parent width, so we need to calculate this box's
  // width before laying out its children.
  calculateWidth(container);
  // Determine where the box is located within its container.
  calculatePosition(container, xCursor, yCursor, shouldRenderBelow);
  // Recursively lay out the children of this box.
  layoutChildren(container.content.width - container.padding.left -
                 container.padding.right);
  // If an explicit height is available, overwrite the height set by the
  // children
  setHeight();
}

bool isBlockLike(style::DisplayType display_type) {
  return display_type == style::Block || display_type == style::Flex;
}

bool isInlineLike(style::DisplayType display_type) {
  return display_type == style::Inline || display_type == style::FlexChild;
}

void LayoutElement::calculateWidth(Dimensions container) {
  int paddingLeft =
      std::stoi(getStyleValue(constants::css_properties::PADDING_LEFT, "0"));
  int paddingRight =
      std::stoi(getStyleValue(constants::css_properties::PADDING_RIGHT, "0"));
  int borderLeft = std::stoi(
      getStyleValue(constants::css_properties::BORDER_LEFT_WIDTH, "0"));
  int borderRight = std::stoi(
      getStyleValue(constants::css_properties::BORDER_RIGHT_WIDTH, "0"));
  int marginLeft =
      std::stoi(getStyleValue(constants::css_properties::MARGIN_LEFT, "0"));
  int marginRight =
      std::stoi(getStyleValue(constants::css_properties::MARGIN_RIGHT, "0"));
  int width;
  if (get_display_type() == style::Text) {
    width = text_render::getTextWidth(this);
  } else {
    width = std::stoi(getStyleValue(constants::css_properties::WIDTH, "-1"));
  }

  int total = paddingLeft + paddingRight + std::max(marginLeft, 0) +
              std::max(marginRight, 0) + borderLeft + borderRight +
              std::max(width, 0);

  // If width is not auto and the total is wider than the container, treat auto
  // margins as 0.
  if (width != -1 && total > container.content.width) {
    if (marginLeft == -1) {
      marginLeft = 0;
    }
    if (marginRight == -1) {
      marginRight = 0;
    }
  }
  // How much needs to be made up to equal the parent container's width
  int underflow = container.content.width - total;
  // int underflow = container.content.width - container.padding.left -
  // container.padding.right - total;

  style::DisplayType dt = get_display_type();
  if (dt == style::Text || dt == style::Inline ||
      (dt == style::FlexChild && width == -1)) {
    width = total;
  }
  if (width == -1) {
    // If width is set to auto, any other auto values become 0.
    if (marginLeft == -1) {
      marginLeft = 0;
    }
    if (marginRight == -1) {
      marginRight = 0;
    }
    if (underflow > 0) {
      width = underflow;
    } else {
      // Width can't be negative. Adjust the right margin instead.
      width = 0;
      marginRight = marginRight + underflow;
    }
  } else if (marginLeft == -1 && marginRight == -1) {
    // If margin-left and margin-right are both auto, their used values are
    // equal.
    marginRight = underflow / 2.0;
    marginLeft = underflow / 2.0;
  } else if (marginLeft == -1) {
    marginLeft = underflow;
  } else if (marginRight == -1) {
    marginRight = underflow;
  } else {
    // If the values are overconstrained, calculate margin_right.
    marginRight = marginRight + underflow;
  }

  dimensions.padding.left = paddingLeft;
  dimensions.padding.right = paddingRight;
  dimensions.margin.left = marginLeft;
  dimensions.margin.right = marginRight;
  dimensions.border.left = borderLeft;
  dimensions.border.right = borderRight;
  dimensions.content.width = width;
}
void LayoutElement::calculatePosition(Dimensions container, int xCursor,
                                      int yCursor, bool shouldRenderBelow) {
  int paddingTop =
      std::stoi(getStyleValue(constants::css_properties::PADDING_TOP, "0"));
  int paddingBottom =
      std::stoi(getStyleValue(constants::css_properties::PADDING_BOTTOM, "0"));
  int borderTop = std::stoi(
      getStyleValue(constants::css_properties::BORDER_TOP_WIDTH, "0"));
  int borderBottom = std::stoi(
      getStyleValue(constants::css_properties::BORDER_BOTTOM_WIDTH, "0"));
  int marginTop =
      std::stoi(getStyleValue(constants::css_properties::MARGIN_TOP, "0"));
  int marginBottom =
      std::stoi(getStyleValue(constants::css_properties::MARGIN_BOTTOM, "0"));

  dimensions.padding.top = paddingTop;
  dimensions.padding.bottom = paddingBottom;
  dimensions.margin.top = marginTop;
  dimensions.margin.bottom = marginBottom;
  dimensions.border.top = borderTop;
  dimensions.border.bottom = borderBottom;

  // The content starts at the start of the container, + the cursor tracking
  // children within the container, + the padding, border, and margin
  int x = container.content.x + xCursor + dimensions.padding.left +
          dimensions.border.left + dimensions.margin.left;
  int y = yCursor + container.content.y + dimensions.padding.top +
          dimensions.border.top + dimensions.margin.top;
  dimensions.content.x = x;
  dimensions.content.y = y;
}

void LayoutElement::layoutChildren(int parentWidth) {
  // Cursor to keep track of where a each child element should render
  // relative to its siblings.
  int xCursor = 0;
  int yCursor = 0;
  int i = 0;
  bool prevElementIsBlock = false;
  int prevElementHeight = 0;
  int availableChildWidth;
  if (isInlineLike(get_display_type())) {
    availableChildWidth = parentWidth;
  } else {
    availableChildWidth = dimensions.content.width;
  }
  for (LayoutElement &child : get_children()) {
    logger::debug(absl::StrFormat("Laying out %d child #%d of %d",
                                  child.get_display_type(), i,
                                  get_display_type()));
    bool currElementIsBlock = isBlockLike(child.get_display_type());
    child.calculateWidth(dimensions);
    // Would adding this child element to the current row put us over the
    // maximum width of the container?
    bool overflow =
        xCursor + child.dimensions.content.width > availableChildWidth;
    // Block elements, or elements following block elements, will render
    // below the previous element.
    bool shouldRenderBelow =
        prevElementIsBlock || currElementIsBlock || overflow;

    // If we are rendering below the previous element, we reset the xCursor
    // and increment the yCursor.
    if (shouldRenderBelow) {
      yCursor += prevElementHeight;
      xCursor = 0;
    }
    i++;
    child.applyLayout(dimensions, xCursor, yCursor, shouldRenderBelow);
    if (!isBlockLike(child.get_display_type())) {
      // Elements can render next to rather than below an inline element so
      // we increment the xCursor
      xCursor += child.dimensions.borderBox().width;
      if (get_display_type() == style::Inline ||
          (get_display_type() == style::FlexChild &&
           getStyleValue(constants::css_properties::WIDTH, "-1") == "-1")) {
        dimensions.content.width += child.dimensions.content.width;
      }
    }
    // If the element is rendering below the previous element, or is the final
    // element in its row, update the container's height
    if (shouldRenderBelow) {
      dimensions.content.height += child.dimensions.marginBox().height;
    }

    // Parent will be at least as big as it's biggest child
    if (child.dimensions.marginBox().height > dimensions.content.height) {
      dimensions.content.height = child.dimensions.marginBox().height;
    }

    prevElementHeight = child.dimensions.marginBox().height;
    prevElementIsBlock = currElementIsBlock;
  }
}

BoxType parseBoxType(const std::string &tag) {
  if (tag == constants::html_tags::IMG) {
    return Img;
  } else if (tag == constants::html_tags::TEXT) {
    return Text;
  } else if (tag == constants::html_tags::BULLET) {
    return Bullet;
  } else {
    return Shape;
  }
}

std::unique_ptr<LayoutElement> build_layout_tree(
    const style::StyledNode &styleTree) {
  std::unique_ptr<LayoutElement> layoutTree(new LayoutElement(
      styleTree.get_node(), styleTree.get_style_values(),
      styleTree.get_display_type(), parseBoxType(styleTree.get_tag())));
  for (auto c : styleTree.get_children()) {
    std::unique_ptr<LayoutElement> childTree = build_layout_tree(c);
    layoutTree->addChild(std::move(childTree));
  }
  return layoutTree;
}

std::unique_ptr<LayoutElement> layout_tree(const style::StyledNode &styleTree,
                                           Dimensions container) {
  logger::info("****** Building layout ******");
  // The layout algorithm expects the container height to start at 0.
  // TODO: Save the initial containing block height, for calculating percent
  // heights.
  container.content.height = 0.0;
  std::unique_ptr<LayoutElement> root = build_layout_tree(styleTree);
  root->applyLayout(container);
  return root;
}
}  // namespace layout
