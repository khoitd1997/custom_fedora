#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace distro_info_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const DistroInfo&  imageConf,
                                                         const toml::table& table);
}  // namespace distro_info_handler
}  // namespace hatter