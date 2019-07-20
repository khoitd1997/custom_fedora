#pragma once

#include <string>

namespace hatter {
namespace logger {
void init();
void info(const std::string& message, const bool isStartAnimation = false);
void warn(const std::string& message);
void skipLine(const int totalLine = 1);
void error(const std::string& message);
}  // namespace logger
}  // namespace hatter