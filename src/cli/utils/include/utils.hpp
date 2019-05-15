#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "toml11/toml.hpp"

#include "ascii_code.hpp"

namespace hatter {
void writeFile(const std::string& s, const std::string& path);
void writeFile(const std::vector<std::string>& lines, const std::string& path);
bool readFile(std::vector<std::string>& lines, const std::string& path);
void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr);

std::string getExeDir(void);

std::string toUpper(std::string_view str);

std::string strJoin(const std::vector<std::string>& v, const std::string& delimiter = ", ");

std::string formatStr(const std::string& rawStr, const std::string& formatCode);

template <typename T>
void appendVector(std::vector<T>& resultVec, const std::vector<T>& targetVec) {
    resultVec.insert(resultVec.end(), targetVec.begin(), targetVec.end());
}

template <typename T>
void removeDuplicateVector(std::vector<T>& v) {
    std::unordered_set<T> s(v.begin(), v.end());
    v.assign(s.begin(), s.end());
}

template <typename T>
void appendUniqueVector(std::vector<T>& resultVec, const std::vector<T>& targetVec) {
    appendVector(resultVec, targetVec);
    removeDuplicateVector(resultVec);
}
}  // namespace hatter