#include "build_process_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "build_process_sanitize.hpp"

#include "raw_config_parser_utils.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace build_process_handler {
namespace {
static const auto kSectionName = "build_process";
}  // namespace

TopSectionErrorReport parse(toml::table&                 rawConfig,
                            const std::filesystem::path& fileDir,
                            BuildProcessConfig&          outConf) {
    TopSectionErrorReport errorReport(kSectionName);

    toml::table rawBuildConf;
    errorReport.add({getTOMLVal(rawConfig, kSectionName, rawBuildConf)});
    if (errorReport || rawBuildConf.empty()) { return errorReport; }

    std::string tempStr;
    errorReport.add({getTOMLVal(rawBuildConf, "custom_mock_script", tempStr)});
    outConf.mockScriptPaths.push_back(fileDir / tempStr);

    errorReport.add({getTOMLVal(rawBuildConf, "enable_custom_cache", outConf.enableCustomCache)});

    if (!errorReport) { errorReport.add(sanitize(outConf, rawBuildConf)); }

    return errorReport;
}

TopSectionErrorReport merge(BuildProcessConfig& resultConf, const BuildProcessConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    for (const auto& filePath : targetConf.mockScriptPaths) {
        auto isDup = false;
        for (const auto& resFilePath : resultConf.mockScriptPaths) {
            if (resFilePath.string() == filePath.string()) {
                isDup = true;
                break;
            }
        }
        if (!isDup) { resultConf.mockScriptPaths.push_back(filePath); }
    }
    mergeAndCheckConflict(errorReport,
                          "enable_custom_cache",
                          resultConf.enableCustomCache,
                          targetConf.enableCustomCache);

    return errorReport;
}
}  // namespace build_process_handler
}  // namespace hatter