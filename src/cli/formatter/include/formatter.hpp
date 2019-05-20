#pragma once

#include <string>

namespace hatter {
namespace formatter {
std::string formatText(const std::string& rawStr, const std::string& formatCode);
std::string formatImportantText(const std::string& rawStr);
std::string formatErrorText(const std::string& rawStr);
std::string formatErrorFileLocation(const std::string& fileName,
                                    const std::string& parentFile = "");
}  // namespace formatter
}  // namespace hatter