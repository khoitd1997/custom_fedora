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

template <typename T>
void mergeAndCheckConflict(TopSectionErrorReport& errorReport,
                           const std::string&     keyName,
                           T&                     dest,
                           const T&               target) {
    if (dest != target) {
        errorReport.add({std::make_shared<SectionMergeConflictError>(
            keyName, std::string{dest}, std::string{target})});
    }
}
template <>
void mergeAndCheckConflict(TopSectionErrorReport& errorReport,
                           const std::string&     keyName,
                           std::string&           dest,
                           const std::string&     target);
}  // namespace hatter