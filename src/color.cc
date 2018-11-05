#include "color.h"

#include "absl/strings/ascii.h"
#include "absl/strings/str_split.h"

#include "constants.h"

std::map<std::string, std::string> COLOR_KEYWORDS{
    {"WHITE", "#FFFFFF"},   {"BLACK", "#000000"},  {"RED", "#FF0000"},
    {"LIME", "#00FF00"},    {"GREEN", "#008000"},  {"BLUE", "#0000FF"},
    {"MAGENTA", "#FF00FF"}, {"PURPLE", "#800080"}, {"ORANGE", "#FFA500"},
    {"YELLOW", "#FFFF00"},  {"CYAN", "#00FFFF"},   {"GRAY", "#808080"},
};

// Function to convert hexadecimal to decimal
int hexadecimalToDecimal(char hexVal[]) {
  int len = strlen(hexVal);
  // Initializing base value to 1, i.e 16^0
  int base = 1;
  int dec_val = 0;
  // Extracting characters as digits from last character
  for (int i = len - 1; i >= 0; i--) {
    // if character lies in '0'-'9', converting
    // it to integral 0-9 by subtracting 48 from
    // ASCII value.
    char hexChar = hexVal[i];
    if (!absl::ascii_isxdigit(hexChar)) {
      throw 20;
    }
    if (hexChar >= '0' && hexChar <= '9') {
      dec_val += (hexChar - 48) * base;
      // incrementing base by power
      base = base * 16;
    }

    // if character lies in 'A'-'F' , converting
    // it to integral 10 - 15 by subtracting 55
    // from ASCII value
    else if (hexChar >= 'A' && hexChar <= 'F') {
      dec_val += (hexChar - 55) * base;
      // incrementing base by power
      base = base * 16;
    }
  }
  return dec_val;
}

// If provided color is a color keyword, converts to hex
std::string maybeParseColorKeywords(const std::string& colorStr) {
  if (COLOR_KEYWORDS.find(colorStr) != COLOR_KEYWORDS.end()) {
    return COLOR_KEYWORDS[colorStr];
  } else {
    return colorStr;
  }
}

// Converts an `rgb(R, G, B)` string to an rgb Color instance
sf::Color parse_rgb(const std::string& colorStr) {
  sf::Color rgb;
  std::vector<std::string> values =
      absl::StrSplit(colorStr.substr(4, colorStr.size() - 5), ',');
  rgb.r = std::stoi(values[0]);
  rgb.g = std::stoi(values[1]);
  rgb.b = std::stoi(values[2]);
  rgb.a = 255;
  return rgb;
}

// Parses a CSS color value (hex, rgb or name) into a Color instance
sf::Color parseColor(std::string colorStr) {
  colorStr = absl::AsciiStrToUpper(colorStr);
  sf::Color rgb;

  if (colorStr == constants::css_font_values::INHERIT ||
      colorStr == constants::css_font_values::TRANSPARENT) {
    return rgb;
  }
  colorStr = maybeParseColorKeywords(colorStr);
  if (colorStr.substr(0, 3) == "RGB") {
    return parse_rgb(colorStr);
  }
  assert(colorStr[0] == '#');
  // If it's an abbreviated hex style, expand it
  if (colorStr.size() == 4) {
    colorStr.insert(1, colorStr.substr(1, 1));
    colorStr.insert(3, colorStr.substr(3, 1));
    colorStr.insert(5, colorStr.substr(5, 1));
  }

  char r_pair[2];
  strcpy(r_pair, colorStr.substr(1, 2).c_str());
  rgb.r = hexadecimalToDecimal(r_pair);

  char g_pair[2];
  strcpy(g_pair, colorStr.substr(3, 2).c_str());
  rgb.g = hexadecimalToDecimal(g_pair);

  char b_pair[2];
  strcpy(b_pair, colorStr.substr(5, 2).c_str());
  rgb.b = hexadecimalToDecimal(b_pair);

  char a_pair[2];
  strcpy(a_pair, "FF");
  rgb.a = hexadecimalToDecimal(a_pair);
  return rgb;
}

// Extract the specified color value for the element
sf::Color getColor(const layout::LayoutElement& box,
                   const std::string& colorType) {
  std::string c = box.getStyleValue(colorType, constants::DEFAULT);
  if (c == constants::DEFAULT) {
    return sf::Color::White;
  }
  sf::Color color = parseColor(c);
  return color;
}
