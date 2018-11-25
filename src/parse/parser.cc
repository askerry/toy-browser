#include <assert.h>
#include <vector>

#include "parser.h"

char BaseParser::nextChar() { return input_[pos_]; };

char BaseParser::lastChar() { return input_[pos_ - 1]; };

bool BaseParser::startsWith(const std::string& str) {
  std::size_t index = input_.rfind(str, pos_) - pos_;
  return index == 0;
};

bool BaseParser::endOfInput() { return pos_ >= input_.size(); };

std::string BaseParser::consumeWhile(std::function<bool(char)> condition) {
  std::string result = "";
  while (!endOfInput() && condition(nextChar())) {
    result += consumeChar();
  }
  return result;
}

char BaseParser::consumeChar() {
  char currChar = input_[pos_];
  pos_ += 1;
  return currChar;
}

void BaseParser::consumeWhitespace() {
  consumeWhile([](char c) { return isspace(c); });
}
