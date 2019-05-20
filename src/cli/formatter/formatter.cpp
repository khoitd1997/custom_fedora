#include "formatter.hpp"

#include <string>

#include "ascii_format.hpp"

namespace hatter {
namespace formatter {
std::string formatText(const std::string& rawStr, const std::string& formatCode) {
    return formatCode + rawStr + formatter::kReset;
}
std::string formatImportantText(const std::string& rawStr) {
    return formatText(rawStr, kImportantWordFormat);
}
std::string formatErrorText(const std::string& rawStr) {
    return formatText(rawStr, kErrorListFormat);
}
std::string formatErrorFileLocation(const std::string& fileName, const std::string& parentFile) {
    const auto includeStr = (parentFile.empty()) ? "" : "(" + parentFile + ")";
    return formatText(fileName, formatter::kErrorFileLocationFormat) +
           formatText(includeStr, formatter::kFaint);
}
}  // namespace formatter
}  // namespace hatter