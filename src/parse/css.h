#ifndef CSS_H
#define CSS_H

#include <iostream>
#include <map>
#include <vector>

#include "parser.h"

namespace css {

typedef std::tuple<int, int, int> Specificity;

class Selector {
  std::string tag_name_;
  std::string id_;
  std::vector<std::string> classes_;

 public:
  Selector(const std::string& tag_name) { tag_name_ = tag_name; };
  Selector(const std::string& tag_name, const std::string& id,
           std::vector<std::string> classes) {
    tag_name_ = tag_name;
    id_ = id;
    classes_ = classes;
  }
  void log();
  Specificity getSpecificity();
  std::string get_tag() { return tag_name_; }
  std::string get_id() { return id_; }
  std::vector<std::string> get_classes() { return classes_; }
};

class Declaration {
  std::string name_;
  std::string value_;

 public:
  Declaration(const std::string& name, const std::string& value) {
    name_ = name;
    value_ = value;
  }
  void log();
  std::string get_name() { return name_; };
  std::string get_value() { return value_; };
};

class Rule {
  std::vector<Selector> selectors_;
  std::vector<Declaration> declarations_;

 public:
  Rule(std::vector<Selector> selectors, std::vector<Declaration> declarations) {
    selectors_ = selectors;
    declarations_ = declarations;
  };
  std::vector<Declaration> get_declarations() { return declarations_; }
  std::vector<Selector> get_selectors() { return selectors_; };
};

class StyleSheet {
  std::vector<Rule> rules_;

 public:
  StyleSheet(std::vector<Rule> rules) { rules_ = rules; }
  std::vector<Rule> get_rules() const;
};

class CSSParser : public BaseParser {
 private:
  Rule parseRule();
  std::vector<Selector> parseSelectors();
  std::vector<Declaration> parseDeclarations();
  Declaration parseDeclaration();
  Selector parseSimpleSelector();
  void parseComment();
  std::string parseIdentifier();
  std::string parseProperty();
  std::string parseValue();

 public:
  CSSParser(int pos, const std::string& input);
  std::vector<Rule> parseRules();
};

std::unique_ptr<StyleSheet const> parseCss(const std::string& source);
}  // namespace css

#endif
