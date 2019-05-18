#include "package_parser.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "toml_utils.hpp"
#include "unknown_value_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
enum class PackageType { RPM, RPM_GROUP };

namespace internal {
std::optional<std::shared_ptr<RepoNoLinkError>> checkValidPackage(const Repo& repo) {
    if (repo.baseurl.empty() && repo.metaLink.empty()) {
        return std::make_shared<RepoNoLinkError>();
    }
    return {};
}
}  // namespace internal

std::vector<std::shared_ptr<HatterParserError>> sanitize(const PackageSet&  pkgSetConf,
                                                         const toml::table& table,
                                                         const PackageType& pkgType) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(*error); }

    return errors;
}

std::optional<SubSectionErrorReport> getSubSection(const std::string  keyName,
                                                   toml::table&       rawPkgConfig,
                                                   PackageSet&        pkgSet,
                                                   const PackageType& pkgType) {
    toml::table           pkgSetTable;
    SubSectionErrorReport errorReport(keyName);
    bool                  hasError = false;

    hasError |= processError(errorReport, getTOMLVal(rawPkgConfig, keyName, pkgSetTable));
    if (pkgSetTable.size() > 0) {
        hasError |=
            processError(errorReport, getTOMLVal(pkgSetTable, "install", pkgSet.installList));
        hasError |= processError(errorReport, getTOMLVal(pkgSetTable, "remove", pkgSet.removeList));

        if (!hasError) {
            hasError |= processError(errorReport, sanitize(pkgSet, pkgSetTable, pkgType));
        }
    }

    if (hasError) { return errorReport; }

    return {};
}

std::optional<TopSectionErrorReport> get(toml::table& rawConfig, PackageConfig& pkgConfig) {
    TopSectionErrorReport errorReport("package");
    bool                  hasError = false;

    toml::table rawPkgConfig;
    hasError |= processError(errorReport, getTOMLVal(rawConfig, "package", rawPkgConfig));
    if (hasError || rawPkgConfig.empty()) { return errorReport; }

    // parse rpm
    hasError |= processError(errorReport,
                             getSubSection("rpm", rawPkgConfig, pkgConfig.rpm, PackageType::RPM));

    // parse rpm group
    hasError |= processError(
        errorReport,
        getSubSection("rpm_group", rawPkgConfig, pkgConfig.rpmGroup, PackageType::RPM_GROUP));

    if (hasError) { return errorReport; }

    return {};
}

std::optional<SectionMergeErrorReport> merge(PackageConfig&       resultConf,
                                             const PackageConfig& targetConf) {
    SectionMergeErrorReport errorReport("package");
    auto                    mergeHasError = false;

    if (mergeHasError) { return errorReport; }

    return {};
}
}  // namespace hatter