#ifndef _RAW_CONFIG_PARSER_HPP
#define _RAW_CONFIG_PARSER_HPP
#include <string>

#include "toml11/toml.hpp"
namespace hatter {
struct BasicConfig {
    std::string mockEnvVersion;
    std::string imageVersion;
    std::string kickstartTag;
    std::string osName;
    std::string postBuildScript;
};
bool getBasicConfig(const toml::table& rawConfig, BasicConfig& basicConfig);
}  // namespace hatter
#endif