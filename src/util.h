#ifndef B_UTIL_H
#define B_UTIL_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace logger {

inline void log(std::vector<std::string> strings,
                std::string prefix = "DEBUG") {
  std::cout << prefix << ": ";
  for (auto s : strings) {
    std::cout << s << ", ";
  }
  std::cout << std::endl;
}

inline void log(std::string string, std::string prefix = "DEBUG") {
  std::cout << prefix << ": " << string << std::endl;
}
inline void debug(std::vector<std::string> strings) { log(strings, "DEBUG"); }

inline void debug(std::string string) { log(string, "DEBUG"); }
inline void info(std::vector<std::string> strings) { log(strings, "INFO"); }

inline void info(std::string string) { log(string, "INFO"); }

inline void warn(std::vector<std::string> strings) { log(strings, "WARN"); }

inline void warn(std::string string) { log(string, "WARN"); }
inline void error(std::vector<std::string> strings) { log(strings, "ERROR"); }

inline void error(std::string string) { log(string, "ERROR"); }

}  // namespace logger

namespace io {
inline std::string readFile(const std::string& filename) {
  std::string file_contents;
  std::string line;
  std::ifstream f(filename);
  if (f.is_open()) {
    while (getline(f, line)) {
      file_contents += line;
    }
    f.close();
  } else {
    logger::error("Unable to open file: " + filename);
  }
  return file_contents;
}
}  // namespace io

#endif
