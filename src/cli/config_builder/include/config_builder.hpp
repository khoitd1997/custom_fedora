#ifndef _CONFIG_BUILDER_HPP
#define _CONFIG_BUILDER_HPP

#include <string>

#include "raw_config_parser.hpp"

namespace hatter {
void buildMockConfig(const BasicConfig& basicConfig, const std::string& buildDir);
}  // namespace hatter
#endif