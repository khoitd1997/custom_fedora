#pragma once

#include <filesystem>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace build_process_handler {
TopSectionErrorReport parse(toml::table&                 rawConfig,
                            const std::filesystem::path& fileDir,
                            BuildProcessConfig&          outConf);

TopSectionErrorReport merge(BuildProcessConfig& resultConf, const BuildProcessConfig& targetConf);

}  // namespace build_process_handler
}  // namespace hatter