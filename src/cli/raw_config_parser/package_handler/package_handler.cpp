#include "package_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "package_sanitize.hpp"
#include "package_set_handler.hpp"

#include "ascii_code.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_handler {
namespace {
static const auto kSectionName   = "package";
static const auto kSectionFormat = ascii_code::kCyan;
}  // namespace

TopSectionErrorReport parse(toml::table& rawConfig, PackageConfig& pkgConfig) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    toml::table rawPkgConf;
    processError(errorReport, getTOMLVal(rawConfig, kSectionName, rawPkgConf));
    if (errorReport || rawPkgConf.empty()) { return errorReport; }

    processError(errorReport, package_set_handler::parse(rawPkgConf, pkgConfig.rpm));
    processError(errorReport, package_set_handler::parse(rawPkgConf, pkgConfig.rpmGroup));

    if (!errorReport) { processError(errorReport, sanitize(pkgConfig, rawPkgConf)); }

    return errorReport;
}

SectionMergeErrorReport merge(PackageConfig& resultConf, const PackageConfig& targetConf) {
    SectionMergeErrorReport errorReport(kSectionName, kSectionFormat);

    appendUniqueVector(resultConf.rpm.installList, targetConf.rpm.installList);
    appendUniqueVector(resultConf.rpm.removeList, targetConf.rpm.removeList);

    appendUniqueVector(resultConf.rpmGroup.installList, targetConf.rpmGroup.installList);
    appendUniqueVector(resultConf.rpmGroup.removeList, targetConf.rpmGroup.removeList);

    return errorReport;
}
}  // namespace package_handler
}  // namespace hatter