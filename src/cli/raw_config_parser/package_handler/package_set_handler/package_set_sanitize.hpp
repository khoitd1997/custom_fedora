#pragma once

#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"
#include "hatter_config_type.hpp"
#include "package_set_handler.hpp"

namespace hatter {
namespace package_set_handler {
struct PackageNotFoundError : public HatterParserError {
    const PackageSet::PackageType pkgType;
    std::vector<std::string>      packages;

    explicit PackageNotFoundError(const PackageSet::PackageType pkgType);

    std::string what() const override;
};
std::shared_ptr<PackageNotFoundError> checkPackageNotFound(const PackageSet&             pkgSet,
                                                           const PackageSet::PackageType pkgType);

std::vector<std::shared_ptr<HatterParserError>> sanitize(const PackageSet&  pkgSet,
                                                         const toml::table& table);
}  // namespace package_set_handler
}  // namespace hatter