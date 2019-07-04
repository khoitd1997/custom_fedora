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
    errorReport.add({getBaseTable(rawConfig, outConf, rawBuildConf)});
    if (errorReport || rawBuildConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(rawBuildConf, outConf.mockScriptPaths, fileDir),
                     getTOMLVal(rawBuildConf, outConf.enableCustomCache)});

    if (!errorReport) { errorReport.add(sanitize(outConf, rawBuildConf)); }

    return errorReport;
}

TopSectionErrorReport merge(BuildProcessConfig& resultConf, const BuildProcessConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    appendUniqueVector(resultConf.mockScriptPaths.value, targetConf.mockScriptPaths.value);
    mergeAndCheckConflict(errorReport, resultConf.enableCustomCache, targetConf.enableCustomCache);

    return errorReport;
}
}  // namespace build_process_handler
}  // namespace hatter