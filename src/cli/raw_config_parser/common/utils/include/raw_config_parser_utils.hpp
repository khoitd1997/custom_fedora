#pragma once

#include <string>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
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