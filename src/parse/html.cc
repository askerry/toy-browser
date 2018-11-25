#include "html.h"

#include <assert.h>
#include <vector>

#include "absl/strings/ascii.h"

#include "../constants.h"
#include "../util.h"

namespace html_parser {

std::vector<dom::TextNode *> HtmlParser::parseTextNodes() {
  std::vector<dom::TextNode *> nodes;
  std::string text;
  while (nextChar() != '<') {
    std::string word =
        consumeWhile([](char c) { return !isspace(c) && c != '<'; });
    dom::TextNode *node = new dom::TextNode(word);
    nodes.push_back(node);
    dom::TextNode *space = new dom::TextNode(" ");
    nodes.push_back(space);
    text += word + ' ';
    consumeWhitespace();
  };
  return nodes;
}

dom::ElementNode *HtmlParser::parseElementNode() {
  // Opening tag and attributes
  assert(consumeChar() == '<');
  std::string opening_tag = parseTagName();
  Attrs attrs = parseAttributes();
  consumeWhitespace();
  // handle self-closing tags
  if (startsWith("/>")) {
    assert(consumeChar() == '/');
    assert(consumeChar() == '>');
    std::vector<dom::Node *> children;
    dom::ElementNode *node = new dom::ElementNode(opening_tag, attrs, children);
    return node;
  }
  assert(consumeChar() == '>');

  // Parse element recursively
  std::vector<dom::Node *> children = parseNodes();
  if (opening_tag == constants::html_tags::LI) {
    Attrs a;
    std::vector<dom::Node *> c;
    dom::ElementNode *bulletNode =
        new dom::ElementNode(constants::html_tags::BULLET, a, c);
    children.insert(children.begin(), bulletNode);
  }
  dom::ElementNode *node = new dom::ElementNode(opening_tag, attrs, children);

  // Closing tag
  assert(consumeChar() == '<');
  assert(consumeChar() == '/');
  std::string closing_tag = parseTagName();
  assert(closing_tag == opening_tag);
  assert(consumeChar() == '>');

  return node;
}

void HtmlParser::parseComment() {
  assert(consumeChar() == '<');
  assert(consumeChar() == '!');
  assert(consumeChar() == '-');
  assert(consumeChar() == '-');
  while (!startsWith("-->")) {
    consumeChar();
  }
  assert(consumeChar() == '-');
  assert(consumeChar() == '-');
  assert(consumeChar() == '>');
}

void HtmlParser::parseSingleLineComment() {
  assert(consumeChar() == '{');
  assert(consumeChar() == '#');
  while (!startsWith("#}")) {
    consumeChar();
  }
  assert(consumeChar() == '#');
  assert(consumeChar() == '}');
}

Attrs HtmlParser::parseAttributes() {
  consumeWhitespace();
  Attrs attrs;
  while (nextChar() != '>' && !startsWith("/>")) {
    consumeWhitespace();
    std::pair<std::string, std::string> kv = parseAttribute();
    attrs[kv.first] = kv.second;
    consumeWhitespace();
  }
  return attrs;
}

std::pair<const std::string &, const std::string &>
HtmlParser::parseAttribute() {
  std::string name = parseTagName();
  assert(consumeChar() == '=');
  std::string value = parseAttrValue();
  absl::RemoveExtraAsciiWhitespace(&name);
  absl::RemoveExtraAsciiWhitespace(&value);
  std::pair<std::string, std::string> kv(name, value);
  return kv;
}

std::string HtmlParser::parseAttrValue() {
  char open_quote = consumeChar();
  assert(open_quote == '"' || open_quote == '\'');
  std::string value =
      consumeWhile([open_quote](char c) { return c != open_quote; });
  assert(consumeChar() == open_quote);
  return value;
}

std::string HtmlParser::parseTagName() {
  std::string tag = consumeWhile([](char c) {
    return c != ' ' && c != '=' && c != '>' && c != '<' && c != '/';
  });
  absl::RemoveExtraAsciiWhitespace(&tag);
  return tag;
}

std::vector<dom::Node *> HtmlParser::parseNodes() {
  consumeWhitespace();
  std::vector<dom::Node *> nodes;
  while (!endOfInput() && !startsWith("</")) {
    consumeWhitespace();
    if (startsWith("<!--")) {
      parseComment();
    } else if (startsWith("{#")) {
      parseSingleLineComment();
    } else if (nextChar() == '<') {
      dom::ElementNode *next_node = parseElementNode();
      nodes.push_back(next_node);
    } else {
      std::vector<dom::TextNode *> next_nodes = parseTextNodes();
      for (auto n : next_nodes) {
        nodes.push_back(n);
      }
    }
    consumeWhitespace();
  }
  consumeWhitespace();
  return nodes;
}

dom::Node *parseHtml(const std::string &source) {
  logger::info("****** Parsing HTML ******");
  HtmlParser parser(0, source);
  return parser.parseNodes()[0];
}
}  // namespace html_parser
