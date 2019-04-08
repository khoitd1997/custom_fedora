#include "raw_config_parser.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "utils.hpp"

namespace hatter {
bool getBasicConfig(const cpptoml::table* rawBasicConfig, BasicConfig& basicConfig) {
    auto isValidConfig = true;
    isValidConfig &= getTOMLVal<std::string>(
        rawBasicConfig, "mock_env_fedora_version", basicConfig.mockEnvVersion, "BASIC");
    isValidConfig &= getTOMLVal<std::string>(
        rawBasicConfig, "image_fedora_version", basicConfig.imageVersion, "BASIC");
    isValidConfig &= getTOMLVal<std::string>(
        rawBasicConfig, "base_kickstart_tag", basicConfig.kickstartTag, "BASIC");
    isValidConfig &=
        getTOMLVal<std::string>(rawBasicConfig, "os_name", basicConfig.osName, "BASIC");
    isValidConfig &= getTOMLVal<std::string>(
        rawBasicConfig, "post_script_dir", basicConfig.postScriptDir, "BASIC");
    return isValidConfig;
}
}  // namespace hatter