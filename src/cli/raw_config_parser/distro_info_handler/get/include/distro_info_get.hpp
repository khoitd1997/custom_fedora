#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace distro_info_handler {
TopSectionErrorReport get(toml::table& rawConfig,
                          DistroInfo&  outConf,
                          toml::table& outRawConf,
                          bool&        isEmpty);
}  // namespace distro_info_handler
}  // namespace hatter