#include "package_set_sanitize.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>

#include "magic_enum.hpp"

#include "ascii_code.hpp"
#include "common_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
namespace {
const auto packageListPath = "/build_temp/package_list.txt";
const auto groupListPath   = "/build_temp/group_list.txt";

// ImageMagick.x86_64
// TODO(kd): Make these changeable

std::string buildRipGrepCommand(const std::string &searchTarget,
                                const std::string &targetFilePath,
                                const bool         noRegex = true) {
    const std::string regexFlag     = (noRegex) ? " " : " -F ";
    const std::string baseRgCommand = "rg " + regexFlag + " -c ";

    return baseRgCommand + searchTarget + " " + targetFilePath;
}

int ripgrepSearchFile(const std::string &searchTarget,
                      const std::string &targetFilePath,
                      const bool         noRegex = true) {
    std::string output;
    auto        rgCommand = buildRipGrepCommand(searchTarget, targetFilePath, noRegex);
    auto        errCode   = execCommand(rgCommand, output);

    if (errCode == 0) {
        return std::stoi(output);
    } else if (errCode == 1) {
        return 0;
    } else {
        throw std::runtime_error("rg failed with message: " + output);
        return 0;
    }
}
}  // namespace

PackageNotFoundError::PackageNotFoundError(const PackageSet::PackageType pkgType)
    : pkgType(pkgType) {}
std::string PackageNotFoundError::what() const {
    return "the following " + std::string(magic_enum::enum_name(pkgType)) +
           "(s) can not be found: " + formatStr(strJoin(packages), ascii_code::kErrorListColor);
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