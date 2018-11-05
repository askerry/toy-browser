#include <assert.h>
#include <vector>

#include "parser.h"

char BaseParser::nextChar() { return this->input[this->pos]; };

char BaseParser::lastChar() { return this->input[this->pos - 1]; };

bool BaseParser::startsWith(const std::string& str) {
  std::size_t index = input.rfind(str, this->pos) - this->pos;
  return index == 0;
};

bool BaseParser::endOfInput() { return this->pos >= this->input.size(); };

std::string BaseParser::consumeWhile(std::function<bool(char)> condition) {
  std::string result = "";
  while (!this->endOfInput() && condition(this->nextChar())) {
    result += this->consumeChar();
  }
  return result;
}

char BaseParser::consumeChar() {
  char currChar = this->input[this->pos];
  this->pos += 1;
  return currChar;
}

void BaseParser::consumeWhitespace() {
  this->consumeWhile([](char c) { return isspace(c); });
}
