#include "raw_config_parser.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>

#include "toml11/toml.hpp"

#include "toml_utils.hpp"
#include "utils.hpp"

static const auto kResetColorCode      = "\033[0m";
static const auto kCyanColorCode       = "\033[38;5;087m";
static const auto kBlueColorCode       = "\033[38;5;12m";
static const auto kYellowColorCode     = "\033[38;5;226m";
static const auto kDarkYellowColorCode = "\033[38;5;220m";
static const auto kDarkGreenColorCode  = "\033[38;5;34m";
static const auto kGreenColorCode      = "\033[38;5;154m";

namespace hatter {
namespace {
// TODO(kd): Refactor this into a log formatting module if needed
void printSection(const std::string& colorCode, const std::string& sectionName) {
    spdlog::info("");
    spdlog::info("parsing section: " + colorCode + hatter::toUpper(sectionName) + kResetColorCode);
}

bool listContain(const std::vector<std::string>& list, const std::string& value) {
    return static_cast<bool>(std::count(list.begin(), list.end(), value));
}

bool checkUnknownOptions(const toml::table& table, const std::vector<std::string>& validOptions) {
    auto isValid = true;
    for (auto const& [key, val] : table) {
        if (!listContain(validOptions, key)) {
            spdlog::error("unknown option: " + key);
            isValid = false;
        }
    }

    return isValid;
}
}  // namespace

BaseConfig::~BaseConfig() {}

toml::table BaseConfig::getBaseTable_(const toml::table& rawConfig,
                                      const std::string& tableName,
                                      const std::string& colorCode) {
    toml::table ret;
    std::string errorMessage = "";

    try {
        ret = toml::get<toml::table>(rawConfig.at(tableName));
        if (ret.size() > 0) { isPresent_ = true; }
    } catch (const toml::type_error& e) {
        errorMessage = "table " + tableName + " has wrong type";
        isValid_     = false;
    } catch (const std::out_of_range& e) {
        // the base table is always optional
    }

    if ((isPresent_ || !isValid_) && (!colorCode.empty())) { printSection(colorCode, tableName); }
    if (!isValid_) { spdlog::error(errorMessage); }
    return ret;
}

DistroInfo::DistroInfo(const toml::table& rawConfig) {
    auto rawDistroInfo = getBaseTable_(rawConfig, "distro_info", kCyanColorCode);

    if (isPresent_) {
        isValid_ &= getTOMLVal(rawDistroInfo, "image_fedora_version", imageVersion);
        isValid_ &= getTOMLVal(rawDistroInfo, "image_fedora_arch", imageArch, true);
        isValid_ &= getTOMLVal(rawDistroInfo, "base_kickstart_tag", kickstartTag, true);
        isValid_ &= getTOMLVal(rawDistroInfo, "base_spin", baseSpin);
        isValid_ &= getTOMLVal(rawDistroInfo, "os_name", osName);

        const std::vector<std::string> validOptions = {"image_fedora_version",
                                                       "image_fedora_arch",
                                                       "base_kickstart_tag",
                                                       "base_spin",
                                                       "os_name"};
        isValid_ &= checkUnknownOptions(rawDistroInfo, validOptions);
    }
}

ImageInfo::ImageInfo(const toml::table& rawConfig) {
    auto rawImageInfo = getBaseTable_(rawConfig, "image_info", kDarkYellowColorCode);

    if (isPresent_) {
        isValid_ &= getTOMLVal(rawImageInfo, "partition_size", partitionSize);
        isValid_ &= getTOMLVal(rawImageInfo, "first_login_script", firstLoginScript, true);
        isValid_ &= getTOMLVal(rawImageInfo, "post_build_script", postBuildScript, true);
        isValid_ &=
            getTOMLVal(rawImageInfo, "post_build_script_no_chroot", postBuildNoRootScript, true);
        isValid_ &= getTOMLVal(rawImageInfo, "user_files", userFiles, true);

        const std::vector<std::string> validOptions = {"partition_size",
                                                       "first_login_script",
                                                       "post_build_script",
                                                       "post_build_script_no_chroot",
                                                       "user_files"};
        isValid_ &= checkUnknownOptions(rawImageInfo, validOptions);
    }
}

BuildProcessConfig::BuildProcessConfig(const toml::table& rawConfig) {
    auto rawBuildConfig = getBaseTable_(rawConfig, "build_process", kDarkGreenColorCode);

    if (isPresent_) {
        isValid_ &= getTOMLVal(rawBuildConfig, "enable_custom_cache", enableCustomCache, true);

        const std::vector<std::string> validOptions = {
            "enable_custom_cache",
        };
        isValid_ &= checkUnknownOptions(rawBuildConfig, validOptions);
    }
}

void Repo::from_toml(const toml::value& v) {
    auto table = v.as_table();

    isValid_ &= getTOMLVal(table,
                           "name",
                           name,
                           false,
                           "repo's name needs to have type string, moving to next repo",
                           "repo's name(string) is undefined, moving to next repo");
    if (isValid_) {
        spdlog::info("parsing repo: " + name);
        isValid_ &= getTOMLVal(table, "display_name", displayName, false);

        isValid_ &= getTOMLVal(table, "metalink", metaLink, true);
        isValid_ &= getTOMLVal(table, "baseurl", baseurl, true);

        if (metaLink.empty() && baseurl.empty()) {
            spdlog::error("repo has neither metalink or baseurl");
            isValid_ = false;
        }

        auto gpgcheckValid = getTOMLVal(table, "gpgcheck", gpgcheck, false);
        isValid_ &= gpgcheckValid;
        if (gpgcheckValid && gpgcheck) {
            isValid_ &= getTOMLVal(table,
                                   "gpgkey",
                                   gpgkey,
                                   false,
                                   "gpgkey needs to have type string when gpgcheck is true",
                                   "gpgkey(string) needs to be defined when gpgcheck is true");
        }

        const std::vector<std::string> validOptions = {
            "name", "display_name", "metalink", "baseurl", "gpgcheck", "gpgkey"};
        isValid_ &= checkUnknownOptions(table, validOptions);
    }
}

RepoConfig::RepoConfig(const toml::table& rawConfig) {
    auto rawRepoConfig = getBaseTable_(rawConfig, "repo", kGreenColorCode);

    if (isPresent_) {
        isValid_ &= getTOMLVal(rawRepoConfig, "standard_repos", standardRepos, true);
        isValid_ &= getTOMLVal(rawRepoConfig, "copr_repos", coprRepos, true);

        getTOMLVal(rawRepoConfig, "custom_repos", customRepos, true);
        isValid_ &= std::accumulate(
            customRepos.begin(), customRepos.end(), true, [](bool isValid, Repo const& repo) {
                return isValid && repo;
            });

        const std::vector<std::string> validOptions = {
            "standard_repos", "copr_repos", "custom_repos"};
        isValid_ &= checkUnknownOptions(rawRepoConfig, validOptions);
    }
}

PackageSet::PackageSet() {}

PackageSet::PackageSet(const toml::table& rawPackageConfig, const std::string& tableName) {
    auto setConfig = getBaseTable_(rawPackageConfig, tableName);

    if (isPresent_) {
        spdlog::info("parsing package set: " + tableName);
        isValid_ &= getTOMLVal(setConfig, "install", installList, true);
        isValid_ &= getTOMLVal(setConfig, "remove", removeList, true);

        const std::vector<std::string> validOptions = {"install", "remove"};
        isValid_ &= checkUnknownOptions(setConfig, validOptions);
    }
}

PackageConfig::PackageConfig(const toml::table& rawConfig) {
    auto rawPackageConfig = getBaseTable_(rawConfig, "package", kYellowColorCode);

    if (isPresent_) {
        rpm      = PackageSet(rawPackageConfig, "rpm");
        rpmGroup = PackageSet(rawPackageConfig, "rpm_group");
        isValid_ &= rpm && rpmGroup;

        const std::vector<std::string> validOptions = {"rpm", "rpm_group"};
        isValid_ &= checkUnknownOptions(rawPackageConfig, validOptions);
    }
}

MiscConfig::MiscConfig(const toml::table& rawConfig) {
    auto rawMiscConfig = getBaseTable_(rawConfig, "misc", kBlueColorCode);

    if (isPresent_) {
        isValid_ &= getTOMLVal(rawMiscConfig, "language", language, true);
        isValid_ &= getTOMLVal(rawMiscConfig, "keyboard", keyboard);
        isValid_ &= getTOMLVal(rawMiscConfig, "timezone", timezone);

        const std::vector<std::string> validOptions = {"language", "keyboard", "timezone"};
        isValid_ &= checkUnknownOptions(rawMiscConfig, validOptions);
    }
}

}  // namespace hatter