#include "error_type.hpp"

#include "ascii_format.hpp"
#include "utils.hpp"

namespace hatter {
HatterParserError::~HatterParserError() {}

SectionMergeConflictError::SectionMergeConflictError(const std::string& keyName,
                                                     const std::string& val1,
                                                     const std::string& val2)
    : keyName(keyName), val1(val1), val2(val2) {}
std::string SectionMergeConflictError::what() const {
    return "merge conflict in " + formatStr(keyName, ascii_format::kImportantWordFormat) + ": " +
           formatStr(val1, ascii_format::kErrorListFormat) + " vs " +
           formatStr(val2, ascii_format::kErrorListFormat);
}
}  // namespace hatter