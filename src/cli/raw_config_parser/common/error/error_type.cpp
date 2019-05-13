#include "error_type.hpp"

namespace hatter {
HatterParserError::~HatterParserError() {}

SectionMergeConflictError::SectionMergeConflictError(const std::string& keyName,
                                                     const std::string& val1,
                                                     const std::string& val2)
    : keyName(keyName), val1(val1), val2(val2) {}
std::string SectionMergeConflictError::what() const {
    return "conflict in " + keyName + ": " + val1 + " vs " + val2;
}
}  // namespace hatter