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
    errorReport.add({getBaseTable(rawConfig, outConf, rawMiscConf)});
    if (errorReport || rawMiscConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(rawMiscConf, outConf.language)});
    errorReport.add({getTOMLVal(rawMiscConf, outConf.keyboard)});
    errorReport.add({getTOMLVal(rawMiscConf, outConf.timezone)});

    if (!errorReport) { errorReport.add(sanitize(outConf, rawMiscConf)); }

    return errorReport;
}

TopSectionErrorReport merge(MiscConfig& resultConf, const MiscConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    mergeAndCheckConflict(errorReport, resultConf.language, targetConf.language);
    mergeAndCheckConflict(errorReport, resultConf.keyboard, targetConf.keyboard);
    mergeAndCheckConflict(errorReport, resultConf.timezone, targetConf.timezone);

    return errorReport;
}
}  // namespace misc_handler
}  // namespace hatter