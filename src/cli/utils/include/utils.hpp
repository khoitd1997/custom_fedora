#ifndef _HATTER_UTILS_HPP
#define _HATTER_UTILS_HPP
#include <memory>
#include <string>
#include <string_view>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "toml11/toml.hpp"

namespace hatter {
void writeFile(const std::string& s, const std::string& path);
void writeFile(const std::vector<std::string>& lines, const std::string& path);
bool readFile(std::vector<std::string>& lines, const std::string& path);
void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr);

std::string getExeDir(void);

std::string toUpper(std::string_view str);
}  // namespace hatter
#endif