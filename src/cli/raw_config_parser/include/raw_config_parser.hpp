#ifndef _RAW_CONFIG_PARSER_HPP
#define _RAW_CONFIG_PARSER_HPP
#include <string>

#include "cpptoml.hpp"
namespace hatter {
struct BasicConfig {
    std::string mockEnvVersion;
    std::string imageVersion;
    std::string kickstartTag;
    std::string osName;
    std::string postScriptDir;
};
bool getBasicConfig(const cpptoml::table* rawBasicConfig, BasicConfig& basicConfig);
}  // namespace hatter
#endif