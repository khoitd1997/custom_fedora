#include "raw_config_parser_utils.hpp"

#include <memory>

#include "utils.hpp"

namespace hatter {
namespace {
std::string buildRipgrepSearchFileCommand(const std::string &searchTarget,
                                          const std::string &targetFilePath,
                                          const bool         useRegex) {
    const std::string regexFlag     = (useRegex) ? " " : " -F ";
    const std::string baseRgCommand = "rg " + regexFlag + " -c ";

    return baseRgCommand + searchTarget + " " + targetFilePath;
}

std::string buildRipgrepSearchOutputCommand(const std::string &searchTarget,
                                            const std::string &cmd,
                                            const bool         useRegex) {
    const std::string regexFlag     = (useRegex) ? " " : " -F ";
    const std::string baseRgCommand = "rg " + regexFlag + " -c ";

    return cmd + " | " + baseRgCommand + searchTarget;
}
}  // namespace

int ripgrepSearchFile(const std::string &searchTarget,
                      const std::string &targetFilePath,
                      const bool         useRegex) {
    std::string output;
    const auto  rgCommand = buildRipgrepSearchFileCommand(searchTarget, targetFilePath, useRegex);
    const auto  errCode   = execCommand(rgCommand, output);

    if (errCode == 0) {
        return std::stoi(output);
    } else if (errCode == 1) {
        return 0;
    } else {
        throw std::runtime_error("rg failed with message: " + output);
        return 0;
    }
}

int ripgrepSearchCmdOutput(const std::string &searchTarget,
                           const std::string &cmd,
                           std::string &      errorOutput,
                           const bool         useRegex) {
    std::string tempOutput;
    const auto  rgCommand = buildRipgrepSearchOutputCommand(searchTarget, cmd, useRegex);
    const auto  errCode   = execCommand(rgCommand, tempOutput);

    if (errCode == 0) {
        return std::stoi(tempOutput);
    } else if (errCode == 1 && tempOutput.empty()) {
        return 0;
    }

    errorOutput = tempOutput;
    return 0;
}

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