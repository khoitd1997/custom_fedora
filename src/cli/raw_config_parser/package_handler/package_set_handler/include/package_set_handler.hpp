#pragma once

#include <filesystem>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace package_set_handler {
SubSectionErrorReport parse(toml::table& rawConfig, PackageSet& pkgSet);

SubSectionErrorReport merge(PackageSet& resultConf, const PackageSet& targetConf);
}  // namespace package_set_handler
}  // namespace hatter