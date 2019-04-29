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
BaseConfig::BaseConfig(const std::string& sectionName) : sectionName(sectionName) {}
BaseConfig::~BaseConfig() {}

void BaseConfig::printSection(const std::string& colorCode) {
    spdlog::info("----------------------------------------");
    spdlog::info("parsing section: " + colorCode + toUpper(sectionName) + kResetColorCode);
}

BasicConfig::BasicConfig(const toml::table& rawConfig) : BaseConfig("basic") {
    printSection(kCyanColorCode);
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

RepoConfig::RepoConfig(const toml::table& rawConfig) : BaseConfig("repo") {
    printSection(kGreenColorCode);
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

bool PackageSet::parse(const toml::table& rawPackageConfig, const std::string& tableName) {
    toml::table setConfig;
    auto        isValid = getTOMLVal<toml::table>(rawPackageConfig, tableName, setConfig, true);

    if (isValid && (setConfig.size() > 0)) {
        spdlog::info("parsing package set: " + tableName);
        isValid &= getTOMLVal<std::vector<std::string>>(setConfig, "install", installList, true);
        isValid &= getTOMLVal<std::vector<std::string>>(setConfig, "remove", removeList, true);
    }
    return isValid;
}

PackageConfig::PackageConfig(const toml::table& rawConfig) : BaseConfig("package") {
    toml::table rawPackageConfig;
    isValid &= getTOMLVal<toml::table>(rawConfig, sectionName, rawPackageConfig, true);

    if (isValid && rawPackageConfig.size() > 0) {
        printSection(kYellowColorCode);
        isValid &= rpm.parse(rawPackageConfig, "rpm");
        isValid &= rpmGroup.parse(rawPackageConfig, "rpm_group");
    }
}
}  // namespace hatter