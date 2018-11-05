#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <functional>
#include <iostream>

#include "../dom.h"
#include "parser.h"

namespace html_parser {

/*
 * Parses HTML into tree of DOM nodes.
 */
class HtmlParser : public BaseParser {
 private:
  dom::ElementNode *parseElementNode();
  void parseComment();
  void parseSingleLineComment();
  std::vector<dom::TextNode *> parseTextNodes();
  Attrs parseAttributes();
  std::pair<const std::string &, const std::string &> parseAttribute();
  std::string parseTagName();
  std::string parseAttrValue();

 public:
  HtmlParser(int pos, const std::string &input) : BaseParser(pos, input){};
  std::vector<dom::Node *> parseNodes();
};

/*
 * Entrypoint to HTML parser.
 */
dom::Node *parseHtml(const std::string &source);
}  // namespace html_parser
#endif
