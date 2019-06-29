#include "utils.hpp"

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace hatter {
void writeFile(const std::string& s, const std::filesystem::path& path) {
    if (std::filesystem::exists(path.parent_path())) {
        std::cout << "writing to " + path.string() << std::endl;
        std::ofstream file(path.string(), std::ofstream::trunc);
        file << s;
    } else {
        throw std::runtime_error("file " + path.string() + " doesn't exist");
    }
}

void writeFile(const std::vector<std::string>& lines, const std::filesystem::path& path) {
    if (std::filesystem::exists(path.parent_path())) {
        std::ofstream file(path.string(), std::ofstream::trunc);
        for (const auto line : lines) { file << line << std::endl; }
    } else {
        throw std::runtime_error("file " + path.string() + " doesn't exist");
    }
}

void appendFile(const std::string& s, const std::filesystem::path& path) {
    if (std::filesystem::exists(path)) {
        std::ofstream file(path.string(), std::ofstream::app);
        file << s;
    } else {
        throw std::runtime_error("file " + path.string() + " doesn't exist");
    }
}

std::string readFile(const std::filesystem::path& path) {
    std::string ret;

    std::ifstream inFile(path.string());
    if (inFile.fail()) { throw std::runtime_error("failed to read file " + path.string()); }

    std::string line;
    while (std::getline(inFile, line)) { strAddLine(ret, {line}); }

    return ret;
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

std::string strJoin(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::string out;
    size_t      count = 0;

    for (const auto& str : strings) {
        out += str;
        if (count < strings.size() - 1) { out += delimiter; }
        ++count;
    }

    return out;
}

void strAddLine(std::string& dest, const std::string& src) { dest += src + "\n"; }
void strAddLine(std::string& dest, const std::vector<std::string>& src) {
    for (const auto& str : src) { strAddLine(dest, str); }
}
void strAddLine(std::string& dest, const std::initializer_list<std::string>& src) {
    strAddLine(dest, std::vector<std::string>{src});
}

bool inStr(const std::string& strToLookFor, const std::string& strToSearchIn) {
    if (strToSearchIn.find(strToLookFor) != std::string::npos) { return true; }
    return false;
}

std::vector<std::string> strSplit(std::string str, const std::string& delimiter, int limit) {
    std::vector<std::string> ret;
    size_t                   pos = 0;

    if (limit == 0) { limit = static_cast<int>(str.length()) + 2; }

    while (((pos = str.find(delimiter)) != std::string::npos) && (limit > 0)) {
        ret.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
        --limit;
    }
    ret.push_back(str);

    return ret;
}

template <>
void appendUniqueVector(std::vector<std::filesystem::path>&       dest,
                        const std::vector<std::filesystem::path>& src) {
    for (const auto& target : dest) {
        for (const auto& result : src) {
            if (target.string() == result.string()) { break; }
        }
        dest.push_back(target);
    }
}

int execCommand(const std::string& cmd) {
    std::string temp;
    (void)temp;
    return execCommand(cmd, temp);
}
int execCommand(const std::string& cmd, std::string& output, const size_t outputBufferSize) {
    std::vector<char> buffer(outputBufferSize);
    auto              pipe = popen(cmd.c_str(), "r");

    if (!pipe) { throw std::runtime_error("popen() failed on command: " + cmd); }

    while (!feof(pipe)) {
        if (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            output += buffer.data();
        }
    }

    return WEXITSTATUS(pclose(pipe));
}
}  // namespace hatter