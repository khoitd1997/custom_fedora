#pragma once

#include <string>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

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
                           ConfigMember<T>&       dest,
                           const ConfigMember<T>& target) {
    if (dest.value != target.value) {
        errorReport.add({std::make_shared<SectionMergeConflictError>(
            dest.keyName, std::string{dest.value}, std::string{target.value})});
    }
}
template <>
void mergeAndCheckConflict(TopSectionErrorReport&           errorReport,
                           ConfigMember<std::string>&       dest,
                           const ConfigMember<std::string>& target);
}  // namespace hatter