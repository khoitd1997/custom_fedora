#pragma once

#include <filesystem>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace distro_info_handler {
TopSectionErrorReport parse(toml::table& rawConfig, DistroInfo& outConf);

TopSectionErrorReport merge(DistroInfo& resultConf, const DistroInfo& targetConf);

}  // namespace distro_info_handler
}  // namespace hatter