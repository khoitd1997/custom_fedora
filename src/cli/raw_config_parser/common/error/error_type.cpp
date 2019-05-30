#include "error_type.hpp"

#include "formatter.hpp"
#include "utils.hpp"

namespace hatter {
HatterParserError::~HatterParserError() {}

SingleKeyError::SingleKeyError(const std::string& keyName) : keyName{keyName} {}
SingleKeyError::~SingleKeyError() {}

SectionMergeConflictError::SectionMergeConflictError(const std::string& keyName,
                                                     const std::string& val1,
                                                     const std::string& val2)
    : SingleKeyError{keyName}, val1{val1}, val2{val2} {}
std::string SectionMergeConflictError::what() const {
    return "merge conflict in " + formatter::formatImportantText(keyName) + ": " +
           formatter::formatErrorText(val1) + " vs " + formatter::formatErrorText(val2);
}
}  // namespace hatter