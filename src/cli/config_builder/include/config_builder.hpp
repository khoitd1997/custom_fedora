#ifndef _CONFIG_BUILDER_HPP
#define _CONFIG_BUILDER_HPP

#include <string>

#include "raw_config_parser.hpp"

namespace hatter {
bool buildMockConfig(const BasicConfig& buildEnvConfig, const std::string& buildDir);
}  // namespace hatter
#endif