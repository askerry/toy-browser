#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <iostream>

/*
 * A base parser used for HTML and CSS parsing.
 */
class BaseParser {
 private:
  int pos_;
  std::string input_;

 protected:
  // Reads the next character without consuming it.
  char nextChar();

  // Reads the last (previous) character
  char lastChar();

  // Returns true if the next characters start with the provided string
  bool startsWith(const std::string& str);

  // Return true if all input has been consumed.
  bool endOfInput();

  // Consumes the next character
  char consumeChar();

  // Consumes until the next non-whitespace character
  void consumeWhitespace();

  // Consumes characters until `condition` function returns false.
  std::string consumeWhile(std::function<bool(char)> condition);

 public:
  BaseParser(int pos, std::string input) {
    pos_ = pos;
    input_ = input;
  };
};

#endif
