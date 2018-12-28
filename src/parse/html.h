// Module for parsing an HTML source string.

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <functional>
#include <iostream>

#include "../dom.h"
#include "parser.h"

namespace html_parser {

// Parses HTML source string into a tree of DOM nodes.
class HtmlParser : public BaseParser {
 private:
  // Parses an ElementNode from the source string.
  std::unique_ptr<dom::ElementNode> parseElementNode();
  // Parses a comment from the HTML source string.
  void parseComment();
  // Parses one or more TextNodes from the source string.
  std::vector<std::unique_ptr<dom::TextNode>> parseTextNodes();
  // Parse attributes of an HTML node (e.g. class from <div class="foo">)
  Attrs parseAttributes();
  // Parse single attribute of an HTML node
  std::pair<const std::string &, const std::string &> parseAttribute();
  // Parse a single word, e.g. "div" or "class"
  std::string parseWord();
  // Parse the value of an HTML node attribute.
  std::string parseAttrValue();

 public:
  HtmlParser(int pos, std::string input) : BaseParser(pos, std::move(input)){};
  std::vector<std::unique_ptr<dom::Node>> parseNodes();
};

// Entrypoint to HTML parser.
std::unique_ptr<dom::Node> parseHtml(const std::string &source);
}  // namespace html_parser
#endif
