#pragma once

#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace misc_handler {
TopSectionErrorReport parse(toml::table& rawConfig, MiscConfig& outConf);

TopSectionErrorReport merge(MiscConfig& resultConf, const MiscConfig& targetConf);

}  // namespace misc_handler
}  // namespace hatter