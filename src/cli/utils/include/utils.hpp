#ifndef _HATTER_UTILS_HPP
#define _HATTER_UTILS_HPP
#include <memory>
#include <string>

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "cpptoml.hpp"

namespace hatter {
template <typename T>
bool getTOMLVal(const cpptoml::table* t,
                const std::string&    keyName,
                T&                    storage,
                const std::string&    section) {
    auto value = t->get_as<T>(keyName);
    if (!value) {
        spdlog::error("[" + section + "] " + keyName + " is either undefined or has wrong type");
        return false;
    }
    // spdlog::debug(keyName + " is parsed successfully");
    storage = *value;
    return true;
}

bool getTOMLTable(const cpptoml::table*            inTable,
                  const std::string&               tableName,
                  std::shared_ptr<cpptoml::table>& outTable);

void        writeFile(const std::string& s, const std::string& path);
std::string getExeDir(void);
}  // namespace hatter
#endif