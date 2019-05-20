#include "package_set_sanitize.hpp"

#include <functional>

#include "magic_enum.hpp"

#include "ascii_format.hpp"
#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
namespace {
const auto packageListPath = "/build_temp/package_list.txt";
const auto groupListPath   = "/build_temp/group_list.txt";
}  // namespace

PackageNotFoundError::PackageNotFoundError(const PackageSet::PackageType pkgType)
    : pkgType(pkgType) {}
std::string PackageNotFoundError::what() const {
    return "the following " +
           formatStr(std::string(magic_enum::enum_name(pkgType)),
                     ascii_format::kImportantWordFormat) +
           "(s) can not be found: " + formatStr(strJoin(packages), ascii_format::kErrorListFormat);
}

std::shared_ptr<PackageNotFoundError> checkPackageNotFound(const PackageSet &pkgSet) {
    std::shared_ptr<PackageNotFoundError> error = nullptr;

    std::string                                     searchFile;
    auto                                            useRegex = false;
    std::function<std::string(const std::string &)> makeSearchTarget;

    switch (pkgSet.packageType) {
        case PackageSet::PackageType::rpm:
            makeSearchTarget = [](const std::string &pkg) -> std::string {
                return "'^" + pkg + "\\.'";
            };
            searchFile = packageListPath;
            useRegex   = true;
            break;

        case PackageSet::PackageType::rpm_group:
            makeSearchTarget = [](const std::string &pkg) -> std::string {
                return "'(" + pkg + ")'";
            };
            searchFile = groupListPath;
            break;

        default:
            throw std::runtime_error("unknown package type");
            break;
    }

    for (const auto &pkg : pkgSet.installList) {
        auto match = ripgrepSearchFile(makeSearchTarget(pkg), searchFile, useRegex);

        // TODO(kd): Handle duplicate case
        if (match < 1) {
            if (!error) { error = std::make_shared<PackageNotFoundError>(pkgSet.packageType); }
            error->packages.push_back(pkg);
        }
    }

    return error;
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const PackageSet & pkgSet,
                                                         const toml::table &table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkPackageNotFound(pkgSet)) { errors.push_back(error); }

    return errors;
}
}  // namespace package_set_handler
}  // namespace hatter