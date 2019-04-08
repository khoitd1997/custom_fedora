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
        rawBasicConfig, "mock_env_fedora_version", basicConfig.mockEnvVersion);
    isValidConfig &=
        getTOMLVal<std::string>(rawBasicConfig, "image_fedora_version", basicConfig.imageVersion);
    isValidConfig &=
        getTOMLVal<std::string>(rawBasicConfig, "base_kickstart_tag", basicConfig.kickstartTag);
    isValidConfig &= getTOMLVal<std::string>(rawBasicConfig, "os_name", basicConfig.osName);
    isValidConfig &=
        getTOMLVal<std::string>(rawBasicConfig, "post_script_dir", basicConfig.postScriptDir);
    return isValidConfig;
}
}  // namespace hatter