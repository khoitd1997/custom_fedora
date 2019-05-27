#pragma once

#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"
#include "hatter_config_type.hpp"
#include "package_handler.hpp"

namespace hatter {
namespace package_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const PackageConfig& pkgConf,
                                                         const toml::table&   table);
}  // namespace package_handler
}  // namespace hatter