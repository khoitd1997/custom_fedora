#include "package_set_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "magic_enum.hpp"

#include "package_set_sanitize.hpp"

#include "toml_utils.hpp"

#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
namespace {
static const auto kSectionFormat = formatter::kErrorSubSectionFormat;
}  // namespace

SubSectionErrorReport parse(toml::table& rawConfig, PackageSet& pkgSet) {
    const auto            sectionName = std::string(magic_enum::enum_name(pkgSet.packageType));
    SubSectionErrorReport errorReport(sectionName, kSectionFormat);

    toml::table rawPkgSet;
    errorReport.add({getBaseTable(rawConfig, pkgSet, rawPkgSet)});
    if (errorReport || rawPkgSet.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(rawPkgSet, pkgSet.installList)});
    errorReport.add({getTOMLVal(rawPkgSet, pkgSet.removeList)});

    if (!errorReport) { errorReport.add(sanitize(pkgSet, rawPkgSet)); }

    return errorReport;
}

SubSectionErrorReport merge(PackageSet& resultConf, const PackageSet& targetConf) {
    SubSectionErrorReport errorReport{resultConf.keyName};

    appendUniqueVector(resultConf.installList.value, targetConf.installList.value);
    appendUniqueVector(resultConf.removeList.value, targetConf.removeList.value);

    return errorReport;
}
}  // namespace package_set_handler
}  // namespace hatter