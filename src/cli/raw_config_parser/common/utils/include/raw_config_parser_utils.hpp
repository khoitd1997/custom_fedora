#pragma once

#include <string>

namespace hatter {
int ripgrepSearchFile(const std::string &searchTarget,
                      const std::string &targetFilePath,
                      const bool         noRegex = true);
}