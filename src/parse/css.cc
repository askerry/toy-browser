#include "css.h"
#include <tuple>

#include "absl/strings/ascii.h"
#include "absl/strings/str_format.h"

#include "../constants.h"
#include "../util.h"

bool validSelectorChar(char c) { return c != '{' && c != '/' && c != ' '; }
bool validPropertyChar(char c) { return isalnum(c) || c == '-' || c == '_'; }
bool validValueChar(char c) { return c != ';' && c != '}'; }

namespace css {

std::vector<Rule> getDefaultTagRules() {
  std::vector<Rule> rules;

  Rule html({Selector(constants::html_tags::HTML)},
            {Declaration(constants::css_properties::FONT_SIZE, "12")});
  rules.push_back(html);

  // h1, 30px
  std::vector<Declaration> h1_declarations = {
      Declaration(constants::css_properties::FONT_SIZE, "30"),
      Declaration(constants::css_properties::FONT_WEIGHT,
                  constants::css_font_values::BOLD),
      Declaration(constants::css_properties::MARGIN_BOTTOM, "10"),
      Declaration(constants::css_properties::MARGIN_TOP, "10"),
  };
  Rule h1({Selector(constants::html_tags::H1)}, h1_declarations);
  rules.push_back(h1);

  // h2, 24px
  std::vector<Declaration> h2_declarations = {
      Declaration(constants::css_properties::FONT_SIZE, "24"),
      Declaration(constants::css_properties::FONT_WEIGHT, "bold"),
      Declaration(constants::css_properties::MARGIN_BOTTOM, "10"),
      Declaration(constants::css_properties::MARGIN_TOP, "10"),
  };
  Rule h2({Selector(constants::html_tags::H2)}, h2_declarations);
  rules.push_back(h2);

  // h3, 18px
  std::vector<Declaration> h3_declarations = {
      Declaration(constants::css_properties::FONT_SIZE, "18"),
      Declaration(constants::css_properties::FONT_WEIGHT, "bold"),
      Declaration(constants::css_properties::MARGIN_BOTTOM, "10"),
      Declaration(constants::css_properties::MARGIN_TOP, "10"),
  };
  Rule h3({Selector(constants::html_tags::H3)}, h3_declarations);
  rules.push_back(h3);

  // h4, 14px
  std::vector<Declaration> h4_declarations = {
      Declaration(constants::css_properties::FONT_SIZE, "14"),
      Declaration(constants::css_properties::FONT_WEIGHT, "bold"),
      Declaration(constants::css_properties::MARGIN_BOTTOM, "10"),
      Declaration(constants::css_properties::MARGIN_TOP, "10"),
  };
  Rule h4({Selector(constants::html_tags::H4)}, h4_declarations);
  rules.push_back(h4);

  std::vector<Declaration> ul_declarations = {
      Declaration(constants::css_properties::MARGIN_BOTTOM, "1em"),
      Declaration(constants::css_properties::MARGIN_TOP, "1em"),
  };
  Rule ul({Selector(constants::html_tags::UL)}, ul_declarations);
  rules.push_back(ul);

  std::vector<Declaration> li_declarations = {
      Declaration(constants::css_properties::PADDING_LEFT, "20"),
  };
  Rule li({Selector(constants::html_tags::LI)}, li_declarations);
  rules.push_back(li);

  std::vector<Declaration> bullet_declarations = {
      Declaration(constants::css_properties::DISPLAY, "inline"),
      Declaration(constants::css_properties::HEIGHT, "1em"),
      Declaration(constants::css_properties::WIDTH, "1em"),
  };
  Rule bullet({Selector("bullet")}, bullet_declarations);
  rules.push_back(bullet);

  std::vector<Declaration> p_declarations = {
      Declaration(constants::css_properties::MARGIN_BOTTOM, "10"),
      Declaration(constants::css_properties::MARGIN_TOP, "10"),
  };
  Rule p({Selector(constants::html_tags::P)}, p_declarations);
  rules.push_back(p);

  std::vector<Declaration> a_declarations = {
      Declaration(constants::css_properties::TEXT_DECORATION,
                  constants::css_font_values::UNDERLINE),
      Declaration(constants::css_properties::COLOR, "#0000EE"),
  };
  Rule a({Selector(constants::html_tags::A)}, a_declarations);
  rules.push_back(a);

  std::vector<Declaration> hr_declarations = {
      Declaration(constants::css_properties::HEIGHT, "1"),
      Declaration(constants::css_properties::BACKGROUND_COLOR, "#000000"),
  };
  Rule hr({Selector(constants::html_tags::HR)}, hr_declarations);
  rules.push_back(hr);

  // Font styling tags
  Rule em({Selector(constants::html_tags::EM)},
          {Declaration(constants::css_properties::FONT_STYLE,
                       constants::css_font_values::ITALIC)});
  rules.push_back(em);
  Rule b({Selector(constants::html_tags::BOLD)},
         {Declaration(constants::css_properties::FONT_WEIGHT,
                      constants::css_font_values::BOLD)});
  rules.push_back(b);
  return rules;
}

std::vector<Rule> StyleSheet::get_rules() const {
  std::vector<Rule> all_rules = getDefaultTagRules();
  // Concatenate the specified rules
  all_rules.insert(all_rules.end(), rules_.begin(), rules_.end());
  return all_rules;
}

void Declaration::log() {
  logger::info(absl::StrFormat("declaration: %s=%s", name_, value_));
}
void Selector::log() {
  std::string str = absl::StrFormat("selector: %s, %s", tag_name_, id_);
  for (auto c : classes_) {
    str += c + ' ';
  }
  logger::info(str);
}

Specificity Selector::getSpecificity() {
  bool hasId = get_id() != "";
  bool hasTag = get_tag() != "";
  Specificity specificity(hasId, get_classes().size(), hasTag);
  return specificity;
}

CSSParser::CSSParser(int pos, const std::string& input)
    : BaseParser(pos, input){};

std::vector<Rule> CSSParser::parseRules() {
  std::vector<Rule> rules;
  while (!endOfInput()) {
    if (startsWith("/*")) {
      parseComment();
    }
    consumeWhitespace();
    rules.push_back(parseRule());
  }
  return rules;
}

std::vector<Selector> CSSParser::parseSelectors() {
  std::vector<Selector> selectors;
  while (!endOfInput()) {
    Selector s = parseSimpleSelector();
    selectors.push_back(s);
    this->consumeWhitespace();
    char next_char = nextChar();
    if (next_char == ',') {
      consumeChar();
      consumeWhitespace();
    } else if (next_char == '{') {
      consumeChar();
      break;
    } else {
      throw std::runtime_error("Invalid CSS selector");
    }
  }
  return selectors;
}

std::vector<Declaration> CSSParser::parseDeclarations() {
  std::vector<Declaration> declarations;
  while (!endOfInput()) {
    consumeWhitespace();
    if (startsWith("/*")) {
      parseComment();
    }
    if (nextChar() == '}') {
      consumeChar();
      break;
    }
    Declaration d = parseDeclaration();
    declarations.push_back(d);
  }
  return declarations;
}

Rule CSSParser::parseRule() {
  return Rule(parseSelectors(), parseDeclarations());
}

std::string CSSParser::parseIdentifier() {
  return consumeWhile(validSelectorChar);
}

std::string CSSParser::parseProperty() {
  return consumeWhile(validPropertyChar);
}

std::string CSSParser::parseValue() { return consumeWhile(validValueChar); }

void CSSParser::parseComment() {
  assert(consumeChar() == '/');
  assert(consumeChar() == '*');
  while (!startsWith("*/")) {
    consumeChar();
  }
  assert(consumeChar() == '*');
  assert(consumeChar() == '/');
  consumeWhitespace();
}

Declaration CSSParser::parseDeclaration() {
  consumeWhitespace();
  std::string name = parseProperty();
  consumeWhitespace();
  assert(consumeChar() == ':');
  consumeWhitespace();
  std::string value = parseValue();
  consumeWhitespace();
  char closing = nextChar();
  assert(closing == ';' || closing == '}');
  if (closing == ';') {
    consumeChar();
  }
  absl::RemoveExtraAsciiWhitespace(&name);
  absl::RemoveExtraAsciiWhitespace(&value);
  Declaration d = Declaration(name, value);
  return d;
}

Selector CSSParser::parseSimpleSelector() {
  std::string tag_name;
  std::string id;
  std::vector<std::string> classes;
  while (!endOfInput()) {
    this->consumeWhitespace();
    char nc = nextChar();
    if (nc == '#') {
      consumeChar();
      id = parseIdentifier();
    } else if (nc == '.') {
      consumeChar();
      classes.push_back(parseIdentifier());
    } else if (nc == '*') {
      consumeChar();
    } else if (validSelectorChar(nc)) {
      tag_name = parseIdentifier();
    } else {
      break;
    }
  }
  Selector selector(tag_name, id, classes);
  return selector;
}

std::unique_ptr<StyleSheet const> parseCss(const std::string& source) {
  logger::info("****** Parsing CSS ******");
  CSSParser parser(0, source);
  std::vector<Rule> rules = parser.parseRules();
  std::unique_ptr<StyleSheet const> stylesheet(new StyleSheet(rules));
  return stylesheet;
}
}  // namespace css
