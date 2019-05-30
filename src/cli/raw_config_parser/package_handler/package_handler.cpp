#include "package_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <iostream>

#include "toml11/toml.hpp"

#include "package_sanitize.hpp"
#include "package_set_handler.hpp"

#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_handler {
namespace {
static const auto kSectionName = "package";
}  // namespace

TopSectionErrorReport parse(toml::table& rawConfig, PackageConfig& pkgConfig) {
    TopSectionErrorReport errorReport(kSectionName);

    toml::table rawPkgConf;
    errorReport.add({getTOMLVal(rawConfig, kSectionName, rawPkgConf)});
    if (errorReport || rawPkgConf.empty()) { return errorReport; }

    errorReport.add({package_set_handler::parse(rawPkgConf, pkgConfig.rpm)});
    errorReport.add({package_set_handler::parse(rawPkgConf, pkgConfig.rpmGroup)});

    if (!errorReport) { errorReport.add(sanitize(pkgConfig, rawPkgConf)); }

    return errorReport;
}

TopSectionErrorReport merge(PackageConfig& resultConf, const PackageConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    appendUniqueVector(resultConf.rpm.installList, targetConf.rpm.installList);
    appendUniqueVector(resultConf.rpm.removeList, targetConf.rpm.removeList);

    appendUniqueVector(resultConf.rpmGroup.installList, targetConf.rpmGroup.installList);
    appendUniqueVector(resultConf.rpmGroup.removeList, targetConf.rpmGroup.removeList);

    return errorReport;
}
}  // namespace package_handler
}  // namespace hatter