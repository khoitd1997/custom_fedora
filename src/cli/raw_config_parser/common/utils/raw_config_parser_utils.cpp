#include "raw_config_parser_utils.hpp"

#include "utils.hpp"

namespace hatter {
namespace {
std::string buildRipGrepCommand(const std::string &searchTarget,
                                const std::string &targetFilePath,
                                const bool         noRegex) {
    const std::string regexFlag     = (noRegex) ? " " : " -F ";
    const std::string baseRgCommand = "rg " + regexFlag + " -c ";

    return baseRgCommand + searchTarget + " " + targetFilePath;
}
}  // namespace

int ripgrepSearchFile(const std::string &searchTarget,
                      const std::string &targetFilePath,
                      const bool         noRegex) {
    std::string output;
    auto        rgCommand = buildRipGrepCommand(searchTarget, targetFilePath, noRegex);
    auto        errCode   = execCommand(rgCommand, output);

    if (errCode == 0) {
        return std::stoi(output);
    } else if (errCode == 1) {
        return 0;
    } else {
        throw std::runtime_error("rg failed with message: " + output);
        return 0;
    }
}
}  // namespace hatter