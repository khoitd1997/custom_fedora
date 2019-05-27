#pragma once

#include <string>

#include "error_report_section_type.hpp"

namespace hatter {
int ripgrepSearchFile(const std::string& searchTarget,
                      const std::string& targetFilePath,
                      const bool         useRegex = true);

int ripgrepSearchCmdOutput(const std::string& searchTarget,
                           const std::string& cmd,
                           std::string&       errorOutput,
                           const bool         useRegex = true);

void mergeAndCheckStrConflict(TopSectionErrorReport& errorReport,
                              const std::string&     keyName,
                              std::string&           dest,
                              const std::string&     target);
}  // namespace hatter