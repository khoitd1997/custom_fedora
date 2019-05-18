#pragma once

#include <string>

namespace hatter {
namespace logger {
void init();
void info(const std::string& message);
void warn(const std::string& message);
void error(const std::string& message);
}  // namespace logger
}  // namespace hatter