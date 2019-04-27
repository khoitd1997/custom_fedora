#include "raw_config_parser.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "toml11/toml.hpp"

#include "utils.hpp"

static const char kResetColorCode[]  = "\033[0m";
static const char kCyanColorCode[]   = "\033[38;5;087m";
static const char kYellowColorCode[] = "\033[38;5;226m";
static const char kGreenColorCode[]  = "\033[38;5;154m";

namespace hatter {
BaseConfig::BaseConfig(const std::string& sectionName, const std::string& colorCode)
    : sectionName(sectionName) {
    spdlog::info("----------------------------------------");
    spdlog::info("parsing section: " + colorCode + toUpper(sectionName) + kResetColorCode);
}

BaseConfig::~BaseConfig() {}

BasicConfig::BasicConfig(const toml::table& rawConfig) : BaseConfig("basic", kCyanColorCode) {
    toml::table rawBasicConfig;
    isValid &= getTOMLTable(rawConfig, sectionName, rawBasicConfig);

    if (isValid) {
        isValid &= getTOMLVal<std::string>(rawBasicConfig, "image_fedora_version", imageVersion);
        isValid &= getTOMLVal<std::string>(rawBasicConfig, "image_fedora_arch", imageArch);
        isValid &= getTOMLVal<std::string>(rawBasicConfig, "base_kickstart_tag", kickstartTag);
        isValid &= getTOMLVal<std::string>(rawBasicConfig, "base_spin", baseSpin);

        isValid &= getTOMLVal<std::string>(rawBasicConfig, "first_login_script", firstLoginScript);
        isValid &= getTOMLVal<std::string>(rawBasicConfig, "post_build_script", postBuildScript);
        isValid &= getTOMLVal<std::string>(
            rawBasicConfig, "post_build_script_no_chroot", postBuildNoRootScript);

        isValid &= getTOMLVal<std::vector<std::string>>(rawBasicConfig, "user_files", userFiles);
    }
}
}  // namespace hatter