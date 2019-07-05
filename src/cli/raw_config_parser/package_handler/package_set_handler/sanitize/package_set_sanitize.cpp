#include "package_set_sanitize.hpp"

#include <filesystem>
#include <functional>

#include "magic_enum.hpp"

#include "build_variable.hpp"
#include "common_sanitize.hpp"
#include "formatter.hpp"
#include "raw_config_parser_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
namespace {}  // namespace
std::shared_ptr<InvalidValueError> checkPackageNotFound(const PackageSet &pkgSet) {
    std::filesystem::path                           searchFile;
    auto                                            useRegex = false;
    std::function<std::string(const std::string &)> makeSearchTarget;

    switch (pkgSet.packageType) {
        case PackageSet::PackageType::rpm:
            makeSearchTarget = [](const std::string &pkg) -> std::string {
                return "'^" + pkg + "\\.'";
            };
            searchFile = build_variable::kPackageListPath;
            useRegex   = true;
            break;

        case PackageSet::PackageType::rpm_group:
            makeSearchTarget = [](const std::string &pkg) -> std::string {
                return "'(" + pkg + ")'";
            };
            searchFile = build_variable::kGroupListPath;
            break;

        default:
            throw std::runtime_error("unknown package type");
            break;
    }

    return checkInvalidValue(pkgSet.keyName, pkgSet.installList.value, searchFile, useRegex);
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