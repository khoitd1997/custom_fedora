#include "raw_config_parser_utils.hpp"

#include <memory>

#include "utils.hpp"

namespace hatter {
template <>
void mergeAndCheckConflict(TopSectionErrorReport &          errorReport,
                           ConfigMember<std::string> &      dest,
                           const ConfigMember<std::string> &target) {
    if (target.value.empty()) { return; }
    if (dest.value.empty()) {
        dest.value = target.value;
        return;
    }
    if (target.value != dest.value) {
        errorReport.add(
            {std::make_shared<SectionMergeConflictError>(dest.keyName, dest.value, target.value)});
    }
    return;
}
}  // namespace hatter