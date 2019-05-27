#pragma once

#include <memory>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace misc_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const MiscConfig&  miscConf,
                                                         const toml::table& table);
}  // namespace misc_handler
}  // namespace hatter