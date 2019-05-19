#include "package_set_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "magic_enum.hpp"

#include "package_set_sanitize.hpp"

#include "ascii_code.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
// namespace {
// // static const auto kSectionFormat = ascii_code::kCyan;
// }  // namespace

SubSectionErrorReport parse(toml::table& rawConfig, PackageSet& pkgSet) {
    const auto            sectionName = std::string(magic_enum::enum_name(pkgSet.packageType));
    SubSectionErrorReport errorReport(sectionName);

    toml::table rawPkgSet;
    processError(errorReport, getTOMLVal(rawConfig, sectionName, rawPkgSet));
    if (errorReport || rawPkgSet.empty()) { return errorReport; }

    processError(errorReport, getTOMLVal(rawPkgSet, "install", pkgSet.installList));
    processError(errorReport, getTOMLVal(rawPkgSet, "remove", pkgSet.removeList));

    if (!errorReport) { processError(errorReport, sanitize(pkgSet, rawPkgSet)); }

    return errorReport;
}
}  // namespace package_set_handler
}  // namespace hatter