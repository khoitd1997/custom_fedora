#include "package_set_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "magic_enum.hpp"

#include "package_set_sanitize.hpp"

#include "toml_utils.hpp"

namespace hatter {
namespace package_set_handler {
namespace {
static const auto kSectionFormat = formatter::kErrorSubSectionFormat;
}  // namespace

SubSectionErrorReport parse(toml::table& rawConfig, PackageSet& pkgSet) {
    const auto            sectionName = std::string(magic_enum::enum_name(pkgSet.packageType));
    SubSectionErrorReport errorReport(sectionName, kSectionFormat);

    toml::table rawPkgSet;
    errorReport.add({getTOMLVal(rawConfig, sectionName, rawPkgSet)});
    if (errorReport || rawPkgSet.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(rawPkgSet, "install", pkgSet.installList)});
    errorReport.add({getTOMLVal(rawPkgSet, "remove", pkgSet.removeList)});

    if (!errorReport) { errorReport.add(sanitize(pkgSet, rawPkgSet)); }

    return errorReport;
}
}  // namespace package_set_handler
}  // namespace hatter