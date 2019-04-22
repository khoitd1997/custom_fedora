#ifndef _RAW_CONFIG_PARSER_HPP
#define _RAW_CONFIG_PARSER_HPP
#include <string>
#include <vector>

#include "toml11/toml.hpp"
namespace hatter {
struct BaseConfig {
    bool              isValid = true;
    const std::string sectionName;

    BaseConfig(const std::string& sectionName, const std::string& colorCode);
    virtual ~BaseConfig() = 0;
};

struct BuildEnvConfig : public BaseConfig {
    std::string mockEnvVersion;
    std::string mockEnvArch;
    std::string osName;
    bool        enableCustomCache = false;

    explicit BuildEnvConfig(const toml::table& rawConfig);
};

struct BasicConfig : public BaseConfig {
    std::string imageVersion;
    std::string imageArch;
    std::string kickstartTag;
    std::string baseSpin;

    std::string firstLoginScript;
    std::string postBuildScript;
    std::string postBuildNoRootScript;

    std::vector<std::string> userFiles;

    explicit BasicConfig(const toml::table& rawConfig);
};
}  // namespace hatter
#endif