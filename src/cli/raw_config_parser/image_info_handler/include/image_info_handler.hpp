#pragma once

#include <filesystem>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace image_info_handler {
TopSectionErrorReport parse(toml::table&                 rawConfig,
                            const std::filesystem::path& fileDir,
                            ImageInfo&                   outConf);

TopSectionErrorReport merge(ImageInfo& resultConf, const ImageInfo& targetConf);

}  // namespace image_info_handler
}  // namespace hatter