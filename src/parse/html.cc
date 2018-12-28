// Module for parsing an HTML source string.

#include "html.h"

#include <assert.h>
#include <vector>

#include "absl/strings/ascii.h"

#include "../constants.h"
#include "../util.h"

namespace html_parser {

std::vector<std::unique_ptr<dom::TextNode>> HtmlParser::parseTextNodes() {
  std::vector<std::unique_ptr<dom::TextNode>> nodes;
  // Consume text nodes until the next opening tag
  while (nextChar() != '<') {
    std::string word =
        consumeWhile([](char c) { return !isspace(c) && c != '<'; });
    std::unique_ptr<dom::TextNode> node(new dom::TextNode(std::move(word)));
    nodes.push_back(std::move(node));
    // TODO: only add in space if next character is a space
    std::unique_ptr<dom::TextNode> space(new dom::TextNode(" "));
    nodes.push_back(std::move(space));
    consumeWhitespace();
  };
  return nodes;
}

std::unique_ptr<dom::ElementNode> HtmlParser::parseElementNode() {
  // Parse opening tag and attributes.
  assert(consumeChar() == '<');
  std::string opening_tag = parseWord();
  Attrs attrs = parseAttributes();
  consumeWhitespace();
  // Handle self-closing elements.
  if (startsWith("/>")) {
    assert(consumeChar() == '/');
    assert(consumeChar() == '>');
    std::vector<std::unique_ptr<dom::Node>> children;
    std::unique_ptr<dom::ElementNode> node(new dom::ElementNode(
        std::move(opening_tag), std::move(attrs), std::move(children)));
    return node;
  }
  assert(consumeChar() == '>');

  // Parse the element's children recursively.
  std::vector<std::unique_ptr<dom::Node>> children = parseNodes();
  // If the current element is a <li>, insert a bullet element.
  if (opening_tag == constants::html_tags::LI) {
    std::unique_ptr<dom::ElementNode> bulletNode(
        new dom::ElementNode(constants::html_tags::BULLET, Attrs(),
                             std::vector<std::unique_ptr<dom::Node>>()));
    children.insert(children.begin(), std::move(bulletNode));
  }
  std::unique_ptr<dom::ElementNode> node(
      new dom::ElementNode(opening_tag, std::move(attrs), std::move(children)));

  // Parse the closing tag.
  assert(consumeChar() == '<');
  assert(consumeChar() == '/');
  // Closing tag should be the same as the opening tag
  assert(parseWord() == opening_tag);
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

Attrs HtmlParser::parseAttributes() {
  consumeWhitespace();
  Attrs attrs;
  // Until we reach a closing, parse attributes
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
  std::string name = parseWord();
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

std::string HtmlParser::parseWord() {
  std::string tag = consumeWhile([](char c) {
    return c != ' ' && c != '=' && c != '>' && c != '/';
  });
  return tag;
}

std::vector<std::unique_ptr<dom::Node>> HtmlParser::parseNodes() {
  consumeWhitespace();
  std::vector<std::unique_ptr<dom::Node>> nodes;
  // While we haven't reached the end of the document or a closing tag,
  // continue to parse from the current position.
  while (!endOfInput() && !startsWith("</")) {
    consumeWhitespace();
    if (startsWith("<!--")) {
      parseComment();
    } else if (nextChar() == '<') {
      nodes.push_back(parseElementNode());
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
  // We assume there is only one root node and thus return the first node
  // in the top-level of the tree.
  return std::move(parser.parseNodes()[0]);
}
}  // namespace html_parser
