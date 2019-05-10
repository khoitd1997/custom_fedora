#include "utils.hpp"

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace hatter {
void writeFile(const std::string& s, const std::string& path) {
    std::ofstream file(path, std::ofstream::trunc);
    file << s;
    file.close();
}

void writeFile(const std::vector<std::string>& lines, const std::string& path) {
    std::ofstream file(path, std::ofstream::trunc);
    for (const auto line : lines) { file << line << std::endl; }
    file.close();
}

bool readFile(std::vector<std::string>& lines, const std::string& path) {
    std::ifstream inFile(path);
    if (inFile.fail()) { return false; }

    std::string line;
    while (std::getline(inFile, line)) { lines.push_back(line); }

    return true;
}

void replacePattern(std::vector<std::string>& lines,
                    const std::string&        regexPattern,
                    const std::string&        replaceStr) {
    std::regex e(regexPattern);
    for (auto& line : lines) { line = std::regex_replace(line, e, replaceStr); }
}

std::string getExeDir(void) {
    char buff[PATH_MAX];
    auto len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len != -1) {
        buff[len] = '\0';
        return std::string(dirname(buff));
    }

    return std::string("");
}

std::string toUpper(std::string_view str) {
    std::string ret(str);
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}

std::string strJoin(const std::vector<std::string>& v, const std::string& delimiter) {
    std::string out;
    if (auto i = v.begin(), e = v.end(); i != e) {
        out += *i++;
        for (; i != e; ++i) out.append(delimiter).append(*i);
    }
    return out;
}
}  // namespace hatter