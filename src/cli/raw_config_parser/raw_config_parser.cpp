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

BasicConfig::BasicConfig(const toml::table& rawConfig) {
    auto rawBasicConfig = getBaseTable_(rawConfig, "basic", kCyanColorCode);

    if (isPresent_) {
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
    auto rawRepoConfig = getBaseTable_(rawConfig, "repo", kGreenColorCode);

    if (isPresent_) {
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
    auto setConfig = getBaseTable_(rawPackageConfig, tableName);

    if (isPresent_) {
        spdlog::info("parsing package set: " + tableName);
        isValid_ &= getTOMLVal<std::vector<std::string>>(setConfig, "install", installList, true);
        isValid_ &= getTOMLVal<std::vector<std::string>>(setConfig, "remove", removeList, true);
    }
}

PackageConfig::PackageConfig(const toml::table& rawConfig) {
    auto rawPackageConfig = getBaseTable_(rawConfig, "package", kYellowColorCode);

    if (isPresent_) {
        rpm      = PackageSet(rawPackageConfig, "rpm");
        rpmGroup = PackageSet(rawPackageConfig, "rpm_group");
        isValid_ &= rpm && rpmGroup;
    }
}
}  // namespace hatter