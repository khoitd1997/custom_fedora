#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>

namespace hatter {
void writeFile(const std::string& s, const std::string& path);
void writeFile(const std::vector<std::string>& lines, const std::string& path);
bool readFile(std::vector<std::string>& lines, const std::string& path);

void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr);

std::string getExeDir(void);

std::string toUpper(std::string_view str);

std::string strJoin(const std::vector<std::string>& strings, const std::string& delimiter = ", ");

bool inStr(const std::string& strToLookFor, const std::string& strToSearchIn);

std::vector<std::string> strSplit(std::string str, const std::string& delimiter, int limit = 0);

template <typename T>
bool inVector(const T& target, const std::vector<T>& list) {
    if (std::find(list.begin(), list.end(), target) != list.end()) { return true; }
    return false;
}

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

int execCommand(const std::string& cmd, std::string& output, const size_t outputBufferSize = 1000);
}  // namespace hatter