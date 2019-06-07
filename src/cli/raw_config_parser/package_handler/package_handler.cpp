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
    errorReport.add({getBaseTable(rawConfig, pkgConfig, rawPkgConf)});
    if (errorReport || rawPkgConf.empty()) { return errorReport; }

    errorReport.add({package_set_handler::parse(rawPkgConf, pkgConfig.rpm),
                     package_set_handler::parse(rawPkgConf, pkgConfig.rpmGroup)});

    if (!errorReport) { errorReport.add(sanitize(pkgConfig, rawPkgConf)); }

    return errorReport;
}

TopSectionErrorReport merge(PackageConfig& resultConf, const PackageConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    errorReport.add({package_set_handler::merge(resultConf.rpm, targetConf.rpm),
                     package_set_handler::merge(resultConf.rpmGroup, targetConf.rpmGroup)});

    return errorReport;
}
}  // namespace package_handler
}  // namespace hatter