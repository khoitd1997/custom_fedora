#include "misc_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "misc_sanitize.hpp"

#include "ascii_code.hpp"
#include "raw_config_parser_utils.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace misc_handler {
namespace {
static const auto kSectionName   = "misc";
static const auto kSectionFormat = ascii_code::kLightMidGreen;
}  // namespace

TopSectionErrorReport parse(toml::table& rawConfig, MiscConfig& outConf) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    toml::table rawMiscConf;
    processError(errorReport, getTOMLVal(rawConfig, kSectionName, rawMiscConf));
    if (errorReport || rawMiscConf.empty()) { return errorReport; }

    processError(errorReport, getTOMLVal(rawMiscConf, "language", outConf.language));
    processError(errorReport, getTOMLVal(rawMiscConf, "keyboard", outConf.keyboard));
    processError(errorReport, getTOMLVal(rawMiscConf, "timezone", outConf.timezone));

    if (!errorReport) { processError(errorReport, sanitize(outConf, rawMiscConf)); }

    return errorReport;
}

SectionMergeErrorReport merge(MiscConfig& resultConf, const MiscConfig& targetConf) {
    SectionMergeErrorReport errorReport(kSectionName, kSectionFormat);

    mergeAndCheckStrConflict(errorReport, "keyboard", resultConf.keyboard, targetConf.keyboard);

    return errorReport;
}
}  // namespace misc_handler
}  // namespace hatter