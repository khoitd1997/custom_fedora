#include "utils.hpp"

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

namespace hatter {
bool getTOMLTable(const toml::table& inTable, const std::string& tableName, toml::table& outTable) {
    try {
        outTable = toml::get<toml::Table>(inTable.at(tableName));
    } catch (const std::out_of_range& e) {
        spdlog::error("Can't get table " + tableName);
        return false;
    }
    return true;
}

void writeFile(const std::string& s, const std::string& path) {
    std::ofstream file(path, std::ofstream::trunc);
    file << s;
    file.close();
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

std::string toUpper(const std::string& str) {
    std::string ret(str);
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}
}  // namespace hatter