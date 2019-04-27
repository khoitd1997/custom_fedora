#ifndef _HATTER_UTILS_HPP
#define _HATTER_UTILS_HPP
#include <memory>
#include <string>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "toml11/toml.hpp"

namespace hatter {
template <typename T>
bool getTOMLVal(const toml::table& t, const std::string& keyName, T& storage) {
    try {
        storage = toml::get<T>(t.at(keyName));
    } catch (const toml::type_error& e) {
        spdlog::error(keyName + " has wrong type");
        return false;
    } catch (const std::out_of_range& e) {
        spdlog::error(keyName + " is undefined");
        return false;
    }
    return true;
}

bool getTOMLTable(const toml::table& inTable, const std::string& tableName, toml::table& outTable);

void writeFile(const std::string& s, const std::string& path);
void writeFile(const std::vector<std::string>& lines, const std::string& path);
bool readFile(std::vector<std::string>& lines, const std::string& path);
void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr);

std::string getExeDir(void);

std::string toUpper(const std::string& str);
}  // namespace hatter
#endif