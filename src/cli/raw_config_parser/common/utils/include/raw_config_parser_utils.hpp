#pragma once

#include <string>

#include "error_report_section_type.hpp"

namespace hatter {
int ripgrepSearchFile(const std::string& searchTarget,
                      const std::string& targetFilePath,
                      const bool         noRegex = true);

int ripgrepSearchCmdOutput(const std::string& searchTarget,
                           const std::string& cmd,
                           std::string&       errorOutput,
                           const bool         noRegex = true);

void mergeAndCheckStrConflict(SectionMergeErrorReport& errorReport,
                              const std::string&       keyName,
                              std::string&             dest,
                              const std::string&       target);
}  // namespace hatter