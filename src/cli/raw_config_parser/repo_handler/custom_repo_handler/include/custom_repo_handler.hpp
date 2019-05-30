#pragma once

#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace custom_repo_handler {
std::vector<SubSectionErrorReport> parse(std::vector<toml::table> rawCustomRepos,
                                         std::vector<CustomRepo>& customRepos);

SubSectionErrorReport merge(std::vector<CustomRepo>& result, const std::vector<CustomRepo>& target);
}  // namespace custom_repo_handler
}  // namespace hatter