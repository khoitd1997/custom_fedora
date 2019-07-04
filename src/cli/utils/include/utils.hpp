#pragma once

#include <algorithm>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_set>

namespace hatter {
[[nodiscard]] std::string getCurrentTime(const std::string& fmt);

[[nodiscard]] std::string buildCommentBlock(const std::vector<std::string>& lines,
                                            const std::string&              commentPrefix);

void writeFile(const std::string& s, const std::filesystem::path& path);
void writeFile(const std::vector<std::string>& lines, const std::filesystem::path& path);
void writeFileWithHeader(const std::string&           content,
                         const std::filesystem::path& path,
                         const std::string&           commentPrefix     = "#",
                         const std::string&           additionalComment = "");
void appendFile(const std::string& s, const std::filesystem::path& path);
[[nodiscard]] std::string readFile(const std::filesystem::path& path);

void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr);

[[nodiscard]] std::string getExeDir(void);

[[nodiscard]] std::string toUpper(std::string str);

[[nodiscard]] std::string strJoin(const std::vector<std::string>& strings,
                                  const std::string&              delimiter = ", ");
template <typename T>
[[nodiscard]] std::string strJoin(const std::vector<T>&                vals,
                                  std::function<std::string(const T&)> func,
                                  const std::string&                   delimiter = ", ") {
    std::vector<std::string> v;
    std::transform(
        std::begin(vals), std::end(vals), std::back_inserter(v), [func](const auto& val) {
            return func(val);
        });
    return strJoin(v, delimiter);
}

void strAddLine(std::string& dest, const std::string& src);
void strAddLine(std::string& dest, const std::vector<std::string>& src);
void strAddLine(std::string& dest, const std::initializer_list<std::string>& src);

void strAddNonEmptyLine(std::string& dest, const std::string& src);
void strAddNonEmptyLine(std::string& dest, const std::vector<std::string>& src);
void strAddNonEmptySection(std::string&       dest,
                           const std::string& content,
                           const std::string& sectionName,
                           const std::string& commentPrefix = "#");

[[nodiscard]] bool inStr(const std::string& strToLookFor, const std::string& strToSearchIn);

[[nodiscard]] std::vector<std::string> strSplit(std::string        str,
                                                const std::string& delimiter,
                                                int                limit = 0);

template <typename T>
[[nodiscard]] bool inVector(const T& target, const std::vector<T>& list) {
    if (std::find(list.begin(), list.end(), target) != list.end()) { return true; }
    return false;
}

template <typename T>
void sortVector(std::vector<T>& list) {
    std::sort(list.begin(), list.end());
}

template <typename T>
void appendVector(std::vector<T>& resultVec, const std::vector<T>& targetVec) {
    resultVec.insert(resultVec.end(), targetVec.begin(), targetVec.end());
}

template <typename T>
[[nodiscard]] std::vector<T> subtractVector(std::vector<T> v1, std::vector<T> v2) {
    std::vector<T> ret;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(ret));

    return ret;
}

template <typename T>
void removeDuplicateVector(std::vector<T>& v) {
    std::unordered_set<T> s(v.begin(), v.end());
    v.assign(s.begin(), s.end());
}

template <typename T>
void appendUniqueVector(std::vector<T>& dest, const std::vector<T>& src) {
    appendVector(dest, src);
    removeDuplicateVector(dest);
}

template <>
void appendUniqueVector(std::vector<std::filesystem::path>&       dest,
                        const std::vector<std::filesystem::path>& src);

int execCommand(const std::string& cmd);
int execCommand(const std::string& cmd, std::string& output, const size_t outputBufferSize = 1000);
}  // namespace hatter