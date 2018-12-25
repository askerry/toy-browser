#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace constants {
static std::string DEFAULT = "";
namespace html_tags {
static std::string HTML = "html";
static std::string HR = "hr";
static std::string DIV = "div";
static std::string P = "p";
static std::string H1 = "h1";
static std::string H2 = "h2";
static std::string H3 = "h3";
static std::string H4 = "h4";
static std::string H5 = "h5";
static std::string H6 = "h6";
static std::string SPAN = "span";
static std::string A = "a";
static std::string EM = "em";
static std::string ITALIC = "i";
static std::string BOLD = "b";
static std::string IMG = "img";
static std::string UL = "ul";
static std::string LI = "li";
static std::string BULLET = "bullet";
static std::string TEXT = "text";
static std::string HEAD = "head";
static std::string STYLE = "style";
static std::string LINK = "link";
static std::string META = "meta";

}  // namespace html_tags
namespace html_attributes {
static std::string ID = "id";
static std::string CLASS = "class";
static std::string SRC = "src";
static std::string HREF = "href";
}  // namespace html_attributes
// The following css properties are currently supported (at least partially)
namespace css_properties {
static std::string DISPLAY = "display";
static std::string FONT_SIZE = "font-size";
static std::string FONT_FAMILY = "font-family";
static std::string FONT_WEIGHT = "font-weight";
static std::string FONT_STYLE = "font-style";
static std::string LINE_HEIGHT = "line-height";
static std::string TEXT_DECORATION = "text-decoration";
static std::string COLOR = "color";
static std::string BACKGROUND_COLOR = "background-color";
static std::string BORDER = "border";
static std::string BORDER_RADIUS = "border-radius";
static std::string BORDER_COLOR = "border-color";
static std::string BORDER_WIDTH = "border-width";
static std::string BORDER_STYLE = "border-style";
static std::string BORDER_LEFT_WIDTH = "border-left-width";
static std::string BORDER_RIGHT_WIDTH = "border-right-width";
static std::string BORDER_TOP_WIDTH = "border-top-width";
static std::string BORDER_BOTTOM_WIDTH = "border-bottom-width";

static std::string MARGIN = "margin";
static std::string MARGIN_TOP = "margin-top";
static std::string MARGIN_BOTTOM = "margin-bottom";
static std::string MARGIN_RIGHT = "margin-right";
static std::string MARGIN_LEFT = "margin-left";
static std::string PADDING = "padding";
static std::string PADDING_TOP = "padding-top";
static std::string PADDING_BOTTOM = "padding-bottom";
static std::string PADDING_RIGHT = "padding-right";
static std::string PADDING_LEFT = "padding-left";
static std::string WIDTH = "width";
static std::string HEIGHT = "height";

}  // namespace css_properties
namespace css_font_values {
static std::string BOLD = "bold";
static std::string ITALIC = "italic";
static std::string UNDERLINE = "underline";
static std::string INHERIT = "inherit";
static std::string TRANSPARENT = "transparent";
}  // namespace css_font_values
namespace css_display_types {
static std::string TEXT = "text";
static std::string INLINE = "inline";
static std::string BLOCK = "block";
static std::string INLINE_BLOCK = "inline-block";
static std::string FLEX = "flex";
static std::string FLEX_CHILD = "flex-child";
static std::string NONE = "none";
}  // namespace css_display_types
}  // namespace constants

#endif
