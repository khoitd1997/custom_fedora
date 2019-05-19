#pragma once

#include <filesystem>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace package_handler {
TopSectionErrorReport parse(toml::table& rawConfig, PackageConfig& pkgConfig);

SectionMergeErrorReport merge(PackageConfig& resultConf, const PackageConfig& targetConf);
}  // namespace package_handler
}  // namespace hatter