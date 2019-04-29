#include "raw_config_parser.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "toml11/toml.hpp"

#include "toml_utils.hpp"
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
    isValid &= getTOMLVal<toml::table>(rawConfig, sectionName, rawBasicConfig);

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

void Repo::from_toml(const toml::value& v) {
    auto table   = v.as_table();
    auto isValid = true;

    isValid &= getTOMLVal<std::string>(table,
                                       "name",
                                       name,
                                       false,
                                       "repo's name needs to have type string, moving to next repo",
                                       "repo's name(string) is undefined, moving to next repo");
    if (isValid) {
        spdlog::info("parsing repo: " + name);
        isValid &= getTOMLVal<std::string>(table, "display_name", displayName, false);

        isValid &= getTOMLVal<std::string>(table, "metalink", metaLink, true);
        isValid &= getTOMLVal<std::string>(table, "baseurl", baseurl, true);

        if (metaLink.empty() && baseurl.empty()) {
            spdlog::error("repo has neither metalink or baseurl");
            isValid = false;
        }

        isValid &= getTOMLVal<bool>(table, "gpgcheck", gpgcheck, false);
        if (gpgcheck) {
            isValid &=
                getTOMLVal<std::string>(table,
                                        "gpgkey",
                                        gpgkey,
                                        false,
                                        "gpgkey needs to have type string when gpgcheck is true",
                                        "gpgkey(string) needs to be defined when gpgcheck is true");
        }

        if (!isValid) { throw std::out_of_range("failed to parse repo table"); }
    }
}

RepoConfig::RepoConfig(const toml::table& rawConfig) : BaseConfig("repo", kGreenColorCode) {
    toml::table rawRepoConfig;
    isValid &= getTOMLVal<toml::table>(rawConfig, sectionName, rawRepoConfig);

    if (isValid) {
        isValid &= getTOMLVal<std::vector<std::string>>(
            rawRepoConfig, "standard_repos", standardRepos, true);
        isValid &=
            getTOMLVal<std::vector<std::string>>(rawRepoConfig, "copr_repos", coprRepos, true);

        isValid &= getTOMLVal<std::vector<Repo>>(rawRepoConfig, "custom_repos", customRepos, true);
    }
}
}  // namespace hatter