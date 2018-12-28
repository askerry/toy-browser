#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace constants {
constexpr const char* DEFAULT = "";
namespace html_tags {
constexpr const char* HTML = "html";
constexpr const char* HR = "hr";
constexpr const char* DIV = "div";
constexpr const char* P = "p";
constexpr const char* H1 = "h1";
constexpr const char* H2 = "h2";
constexpr const char* H3 = "h3";
constexpr const char* H4 = "h4";
constexpr const char* H5 = "h5";
constexpr const char* H6 = "h6";
constexpr const char* SPAN = "span";
constexpr const char* A = "a";
constexpr const char* EM = "em";
constexpr const char* ITALIC = "i";
constexpr const char* BOLD = "b";
constexpr const char* IMG = "img";
constexpr const char* UL = "ul";
constexpr const char* LI = "li";
constexpr const char* BULLET = "bullet";
constexpr const char* TEXT = "text";
constexpr const char* HEAD = "head";
constexpr const char* STYLE = "style";
constexpr const char* LINK = "link";
constexpr const char* META = "meta";

}  // namespace html_tags
namespace html_attributes {
constexpr const char* ID = "id";
constexpr const char* CLASS = "class";
constexpr const char* SRC = "src";
constexpr const char* HREF = "href";
}  // namespace html_attributes
// The following css properties are currently supported (at least partially)
namespace css_properties {
constexpr const char* DISPLAY = "display";
constexpr const char* FONT_SIZE = "font-size";
constexpr const char* FONT_FAMILY = "font-family";
constexpr const char* FONT_WEIGHT = "font-weight";
constexpr const char* FONT_STYLE = "font-style";
constexpr const char* LINE_HEIGHT = "line-height";
constexpr const char* TEXT_DECORATION = "text-decoration";
constexpr const char* COLOR = "color";
constexpr const char* BACKGROUND_COLOR = "background-color";
constexpr const char* BORDER = "border";
constexpr const char* BORDER_RADIUS = "border-radius";
constexpr const char* BORDER_COLOR = "border-color";
constexpr const char* BORDER_WIDTH = "border-width";
constexpr const char* BORDER_STYLE = "border-style";
constexpr const char* BORDER_LEFT_WIDTH = "border-left-width";
constexpr const char* BORDER_RIGHT_WIDTH = "border-right-width";
constexpr const char* BORDER_TOP_WIDTH = "border-top-width";
constexpr const char* BORDER_BOTTOM_WIDTH = "border-bottom-width";

constexpr const char* MARGIN = "margin";
constexpr const char* MARGIN_TOP = "margin-top";
constexpr const char* MARGIN_BOTTOM = "margin-bottom";
constexpr const char* MARGIN_RIGHT = "margin-right";
constexpr const char* MARGIN_LEFT = "margin-left";
constexpr const char* PADDING = "padding";
constexpr const char* PADDING_TOP = "padding-top";
constexpr const char* PADDING_BOTTOM = "padding-bottom";
constexpr const char* PADDING_RIGHT = "padding-right";
constexpr const char* PADDING_LEFT = "padding-left";
constexpr const char* WIDTH = "width";
constexpr const char* HEIGHT = "height";

}  // namespace css_properties
namespace css_font_values {
constexpr const char* BOLD = "bold";
constexpr const char* ITALIC = "italic";
constexpr const char* UNDERLINE = "underline";
constexpr const char* INHERIT = "inherit";
constexpr const char* TRANSPARENT = "transparent";
}  // namespace css_font_values
namespace css_display_types {
constexpr const char* TEXT = "text";
constexpr const char* INLINE = "inline";
constexpr const char* BLOCK = "block";
constexpr const char* INLINE_BLOCK = "inline-block";
constexpr const char* FLEX = "flex";
constexpr const char* FLEX_CHILD = "flex-child";
constexpr const char* NONE = "none";
}  // namespace css_display_types
}  // namespace constants

#endif
