#include "utils.hpp"

#include <iostream>
#include <string>

namespace hatter {
bool getTOMLTable(const cpptoml::table*            inTable,
                  const std::string&               tableName,
                  std::shared_ptr<cpptoml::table>& outTable) {
    outTable = inTable->get_table(tableName);
    if (!outTable) {
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
}  // namespace hatter