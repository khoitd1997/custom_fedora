#include "misc_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "misc_sanitize.hpp"

#include "raw_config_parser_utils.hpp"
#include "toml_utils.hpp"

namespace hatter {
namespace misc_handler {
namespace {
static const auto kSectionName = "misc";
}  // namespace

TopSectionErrorReport parse(toml::table& rawConfig, MiscConfig& outConf) {
    TopSectionErrorReport errorReport(kSectionName);

    toml::table rawMiscConf;
    processError(errorReport, getTOMLVal(rawConfig, kSectionName, rawMiscConf));
    if (errorReport || rawMiscConf.empty()) { return errorReport; }

    processError(errorReport, getTOMLVal(rawMiscConf, "language", outConf.language));
    processError(errorReport, getTOMLVal(rawMiscConf, "keyboard", outConf.keyboard));
    processError(errorReport, getTOMLVal(rawMiscConf, "timezone", outConf.timezone));

    if (!errorReport) { processError(errorReport, sanitize(outConf, rawMiscConf)); }

    return errorReport;
}

TopSectionErrorReport merge(MiscConfig& resultConf, const MiscConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    mergeAndCheckStrConflict(errorReport, "language", resultConf.language, targetConf.language);
    mergeAndCheckStrConflict(errorReport, "keyboard", resultConf.keyboard, targetConf.keyboard);
    mergeAndCheckStrConflict(errorReport, "timezone", resultConf.timezone, targetConf.timezone);

    return errorReport;
}
}  // namespace misc_handler
}  // namespace hatter