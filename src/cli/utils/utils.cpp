#include "utils.hpp"

#include <iostream>
#include <string>

namespace hatter {
void writeFile(const std::string& s, const std::string& path) {
    std::ofstream file(path, std::ofstream::trunc);
    file << s;
    file.close();
}
}  // namespace hatter