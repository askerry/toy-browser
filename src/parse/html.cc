#include "html.h"

#include <assert.h>
#include <vector>

#include "absl/strings/ascii.h"

#include "../constants.h"
#include "../util.h"

namespace html_parser {

std::vector<std::unique_ptr<dom::TextNode>> HtmlParser::parseTextNodes() {
  std::vector<std::unique_ptr<dom::TextNode>> nodes;
  std::string text;
  while (nextChar() != '<') {
    std::string word =
        consumeWhile([](char c) { return !isspace(c) && c != '<'; });
    std::unique_ptr<dom::TextNode> node(new dom::TextNode(word));
    nodes.push_back(std::move(node));
    std::unique_ptr<dom::TextNode> space(new dom::TextNode(" "));
    nodes.push_back(std::move(space));
    text += word + ' ';
    consumeWhitespace();
  };
  return nodes;
}

std::unique_ptr<dom::ElementNode> HtmlParser::parseElementNode() {
  // Opening tag and attributes
  assert(consumeChar() == '<');
  std::string opening_tag = parseTagName();
  Attrs attrs = parseAttributes();
  consumeWhitespace();
  // handle self-closing tags
  if (startsWith("/>")) {
    assert(consumeChar() == '/');
    assert(consumeChar() == '>');
    std::vector<std::unique_ptr<dom::Node>> children;
    std::unique_ptr<dom::ElementNode> node(
        new dom::ElementNode(opening_tag, attrs, std::move(children)));
    return node;
  }
  assert(consumeChar() == '>');

  // Parse element recursively
  std::vector<std::unique_ptr<dom::Node>> children = parseNodes();
  if (opening_tag == constants::html_tags::LI) {
    Attrs a;
    std::vector<std::unique_ptr<dom::Node>> c;
    std::unique_ptr<dom::ElementNode> bulletNode(
        new dom::ElementNode(constants::html_tags::BULLET, a, std::move(c)));
    children.insert(children.begin(), std::move(bulletNode));
  }
  std::unique_ptr<dom::ElementNode> node(
      new dom::ElementNode(opening_tag, attrs, std::move(children)));

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

std::vector<std::unique_ptr<dom::Node>> HtmlParser::parseNodes() {
  consumeWhitespace();
  std::vector<std::unique_ptr<dom::Node>> nodes;
  while (!endOfInput() && !startsWith("</")) {
    consumeWhitespace();
    if (startsWith("<!--")) {
      parseComment();
    } else if (startsWith("{#")) {
      parseSingleLineComment();
    } else if (nextChar() == '<') {
      std::unique_ptr<dom::ElementNode> next_node = parseElementNode();
      nodes.push_back(std::move(next_node));
    } else {
      std::vector<std::unique_ptr<dom::TextNode>> next_nodes = parseTextNodes();
      for (auto &n : next_nodes) {
        nodes.push_back(std::move(n));
      }
    }
    consumeWhitespace();
  }
  consumeWhitespace();
  return nodes;
}

std::unique_ptr<dom::Node> parseHtml(const std::string &source) {
  logger::info("****** Parsing HTML ******");
  HtmlParser parser(0, source);
  return std::move(parser.parseNodes()[0]);
}
}  // namespace html_parser
