#include "raw_config_parser.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <iostream>
#include <numeric>

#include "toml11/toml.hpp"

#include "toml_utils.hpp"
#include "utils.hpp"

static const auto kResetColorCode  = "\033[0m";
static const auto kCyanColorCode   = "\033[38;5;087m";
static const auto kYellowColorCode = "\033[38;5;226m";
static const auto kGreenColorCode  = "\033[38;5;154m";

namespace hatter {
namespace {
// TODO(kd): Refactor this into a log formatting module if needed
void printSection(const std::string& colorCode, const std::string& sectionName) {
    spdlog::info("----------------------------------------");
    spdlog::info("parsing section: " + colorCode + hatter::toUpper(sectionName) + kResetColorCode);
}
}  // namespace

BaseConfig::~BaseConfig() {}

BasicConfig::BasicConfig(const toml::table& rawConfig) {
    const auto sectionName = "basic";
    printSection(kCyanColorCode, sectionName);
    toml::table rawBasicConfig;
    isValid_ &= getTOMLVal<toml::table>(rawConfig, sectionName, rawBasicConfig);

    if (isValid_) {
        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "image_fedora_version", imageVersion);
        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "image_fedora_arch", imageArch);
        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "base_kickstart_tag", kickstartTag);
        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "base_spin", baseSpin);

        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "first_login_script", firstLoginScript);
        isValid_ &= getTOMLVal<std::string>(rawBasicConfig, "post_build_script", postBuildScript);
        isValid_ &= getTOMLVal<std::string>(
            rawBasicConfig, "post_build_script_no_chroot", postBuildNoRootScript);

        isValid_ &= getTOMLVal<std::vector<std::string>>(rawBasicConfig, "user_files", userFiles);
    }
}

void Repo::from_toml(const toml::value& v) {
    auto table = v.as_table();

    isValid_ &=
        getTOMLVal<std::string>(table,
                                "name",
                                name,
                                false,
                                "repo's name needs to have type string, moving to next repo",
                                "repo's name(string) is undefined, moving to next repo");
    if (isValid_) {
        spdlog::info("parsing repo: " + name);
        isValid_ &= getTOMLVal<std::string>(table, "display_name", displayName, false);

        isValid_ &= getTOMLVal<std::string>(table, "metalink", metaLink, true);
        isValid_ &= getTOMLVal<std::string>(table, "baseurl", baseurl, true);

        if (metaLink.empty() && baseurl.empty()) {
            spdlog::error("repo has neither metalink or baseurl");
            isValid_ = false;
        }

        auto gpgcheckValid = getTOMLVal<bool>(table, "gpgcheck", gpgcheck, false);
        isValid_ &= gpgcheckValid;
        if (gpgcheckValid && gpgcheck) {
            isValid_ &=
                getTOMLVal<std::string>(table,
                                        "gpgkey",
                                        gpgkey,
                                        false,
                                        "gpgkey needs to have type string when gpgcheck is true",
                                        "gpgkey(string) needs to be defined when gpgcheck is true");
        }
    }
}

RepoConfig::RepoConfig(const toml::table& rawConfig) {
    const auto sectionName = "repo";
    printSection(kGreenColorCode, sectionName);
    toml::table rawRepoConfig;
    isValid_ &= getTOMLVal<toml::table>(rawConfig, sectionName, rawRepoConfig);

    if (isValid_) {
        isValid_ &= getTOMLVal<std::vector<std::string>>(
            rawRepoConfig, "standard_repos", standardRepos, true);
        isValid_ &=
            getTOMLVal<std::vector<std::string>>(rawRepoConfig, "copr_repos", coprRepos, true);

        getTOMLVal<std::vector<Repo>>(rawRepoConfig, "custom_repos", customRepos, true);
        isValid_ &= std::accumulate(
            customRepos.begin(), customRepos.end(), true, [](bool isValid, Repo const& repo) {
                return isValid && repo;
            });
    }
}

PackageSet::PackageSet() {}

PackageSet::PackageSet(const toml::table& rawPackageConfig, const std::string& tableName) {
    toml::table setConfig;
    isValid_ = getTOMLVal<toml::table>(rawPackageConfig, tableName, setConfig, true);

    if (isValid_ && (setConfig.size() > 0)) {
        spdlog::info("parsing package set: " + tableName);
        isValid_ &= getTOMLVal<std::vector<std::string>>(setConfig, "install", installList, true);
        isValid_ &= getTOMLVal<std::vector<std::string>>(setConfig, "remove", removeList, true);
    }
}

PackageConfig::PackageConfig(const toml::table& rawConfig) {
    const auto  sectionName = "package";
    toml::table rawPackageConfig;
    isValid_ &= getTOMLVal<toml::table>(rawConfig, sectionName, rawPackageConfig, true);

    if (isValid_ && rawPackageConfig.size() > 0) {
        printSection(kYellowColorCode, sectionName);
        rpm      = PackageSet(rawPackageConfig, "rpm");
        rpmGroup = PackageSet(rawPackageConfig, "rpm_group");
        isValid_ &= rpm && rpmGroup;
    }
}
}  // namespace hatter