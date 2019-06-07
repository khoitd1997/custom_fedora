#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace image_info_handler {
TopSectionErrorReport get(toml::table&                 rawConfig,
                          const std::filesystem::path& fileDir,
                          ImageInfo&                   outConf,
                          toml::table&                 outRawConf,
                          bool&                        isEmpty);
}  // namespace image_info_handler
}  // namespace hatter