#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace build_process_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const BuildProcessConfig& buildConf,
                                                        const toml::table&        table);
}  // namespace build_process_handler
}  // namespace hatter