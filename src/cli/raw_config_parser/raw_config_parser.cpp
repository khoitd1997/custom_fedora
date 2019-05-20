#include "raw_config_parser.hpp"

#include <filesystem>
#include <iostream>

#include "logger.hpp"

#include "error_report_file_type.hpp"
#include "misc_handler.hpp"
#include "package_handler.hpp"
#include "repo_handler.hpp"
#include "toml_utils.hpp"

// static const auto kResetColorCode      = "\033[0m";
// static const auto kCyanColorCode       = "\033[38;5;087m";
// static const auto kBlueColorCode       = "\033[38;5;12m";
// static const auto kYellowColorCode     = "\033[38;5;226m";
// static const auto kErrorListFormatColorCode = "\033[38;5;220m";
// static const auto kDarkGreenColorCode  = "\033[38;5;34m";
// static const auto kGreenColorCode      = "\033[38;5;154m";

namespace hatter {
FileErrorReport getFile(const std::filesystem::path& filePath,
                        const std::string&           parentFileName,
                        FullConfig&                  fullConfig) {
    auto currDirectory = filePath.parent_path().string();
    auto currFileName  = filePath.filename().string();

    // TODO(kd): error handling here
    auto rawConfig = toml::parse(filePath);

    // TODO(kd): error handling here
    std::vector<std::string> includeFiles;
    // getTOMLVal(rawConfig, "include_files", includeFiles);
    std::cout << "current file: " << filePath << std::endl;

    FileSectionErrorReport fileSectionReport(currFileName, parentFileName);

    processError(fileSectionReport, repo_handler::parse(rawConfig, fullConfig.repoConfig));
    processError(fileSectionReport, package_handler::parse(rawConfig, fullConfig.packageConfig));
    processError(fileSectionReport, misc_handler::parse(rawConfig, fullConfig.miscConfig));

    std::cout << "Config:" << std::endl;
    for (const auto& package : fullConfig.packageConfig.rpm.installList) {
        std::cout << package << std::endl;
    }

    for (const auto& childFile : includeFiles) {
        auto childPath = std::filesystem::path(
            (currDirectory == "") ? childFile : currDirectory + "/" + childFile);
        auto childFileName = childPath.filename().string();
        auto childTable    = toml::parse(childPath.string());

        FullConfig childConf;
        auto       childErrorReport = getFile(childPath, currFileName, childConf);

        if (!fileSectionReport && !childErrorReport) {
            FileMergeErrorReport fileMergeErrorReport(currFileName, childFileName);

            processError(fileMergeErrorReport,
                         repo_handler::merge(fullConfig.repoConfig, childConf.repoConfig));
            processError(fileMergeErrorReport,
                         package_handler::merge(fullConfig.packageConfig, childConf.packageConfig));
            processError(fileMergeErrorReport,
                         misc_handler::merge(fullConfig.miscConfig, childConf.miscConfig));

            if (fileMergeErrorReport) { return FileErrorReport(fileMergeErrorReport); }
        }
    }

    return FileErrorReport(fileSectionReport);
}

bool testGetFile(std::filesystem::path& filePath, FullConfig& fullConfig) {
    if (auto fileError = getFile(filePath, "", fullConfig)) {
        auto errors = fileError.what();
        for (const auto& error : errors) { logger::error(error); }

        return true;
    }
    return false;
}

// namespace {
// void printSection(const std::string& colorCode, const std::string& sectionName) {
//     spdlog::info("");
//     spdlog::info("parsing section: " + colorCode + hatter::toUpper(sectionName) +
//     kResetColorCode);
// }

// bool listContain(const std::vector<std::string>& list, const std::string& value) {
//     return static_cast<bool>(std::count(list.begin(), list.end(), value));
// }

// bool checkUnknownOptions(const toml::table& table, const std::vector<std::string>& validOptions)
// {
//     auto isValid = true;
//     for (auto const& [key, val] : table) {
//         if (!listContain(validOptions, key)) {
//             spdlog::error("unknown option: " + key);
//             isValid = false;
//         }
//     }

//     return isValid;
// }

// template <typename T>
// bool checkConfigSame(const std::string& configName, const T& conf1, const T& conf2) {
//     if (conf1 != conf2) {
//         spdlog::error("conflicting {0}: {1} vs {2}", configName, conf1, conf2);
//         return false;
//     }
//     return true;
// }
// }  // namespace

// BaseConfig::~BaseConfig() {}

// toml::table BaseConfig::getBaseTable_(const toml::table& rawConfig,
//                                       const std::string& tableName,
//                                       const std::string& colorCode) {
//     toml::table ret;
//     std::string errorMessage = "";

//     try {
//         ret = toml::get<toml::table>(rawConfig.at(tableName));
//         if (ret.size() > 0) { isPresent_ = true; }
//     } catch (const toml::type_error& e) {
//         errorMessage = "table " + tableName + " has wrong type";
//         isValid_     = false;
//     } catch (const std::out_of_range& e) {
//         // the base table is always optional
//     }

//     if ((isPresent_ || !isValid_) && (!colorCode.empty())) { printSection(colorCode, tableName);
//     } if (!isValid_) { spdlog::error(errorMessage); } return ret;
// }

// toml::table BaseConfig::getBaseTable_(const RawTOMLConfig& rawConfig,
//                                       const std::string&   tableName,
//                                       const std::string&   colorCode) {
//     if (rawConfig) { return getBaseTable_(rawConfig.config, tableName, colorCode); }

//     isPresent_ = false;
//     isValid_   = true;
//     return toml::table();
// }

// DistroInfo::DistroInfo(const RawTOMLConfig& rawConfig) {
//     auto rawDistroInfo = getBaseTable_(rawConfig, "distro_info", kCyanColorCode);

//     if (isPresent_) {
//         isValid_ &= getTOMLVal(rawDistroInfo, "base_kickstart_tag", kickstartTag, true);
//         isValid_ &= getTOMLVal(rawDistroInfo, "base_spin", baseSpin);
//         isValid_ &= getTOMLVal(rawDistroInfo, "os_name", osName);

//         const std::vector<std::string> validOptions = {
//             "base_kickstart_tag", "base_spin", "os_name"};
//         isValid_ &= checkUnknownOptions(rawDistroInfo, validOptions);
//     }
// }

// bool DistroInfo::merge(const DistroInfo& target) {
//     auto isValid = true;
//     if (target.isPresent_) {
//         isValid &= checkConfigSame("base_kickstart_tag", this->kickstartTag,
//         target.kickstartTag); isValid &= checkConfigSame("base_spin", this->baseSpin,
//         target.baseSpin);
//     }

//     return isValid;
// }

// ImageInfo::ImageInfo(const RawTOMLConfig& rawConfig) {
//     auto rawImageInfo = getBaseTable_(rawConfig, "image_info", kErrorListFormatColorCode);

//     if (isPresent_) {
//         isValid_ &= getTOMLVal(rawImageInfo, "partition_size", partitionSize);
//         isValid_ &= getTOMLVal(rawImageInfo, "first_login_script", firstLoginScript, true);
//         isValid_ &= getTOMLVal(rawImageInfo, "post_build_script", postBuildScript, true);
//         isValid_ &=
//             getTOMLVal(rawImageInfo, "post_build_script_no_chroot", postBuildNoRootScript, true);
//         isValid_ &= getTOMLVal(rawImageInfo, "user_files", userFiles, true);

//         const std::vector<std::string> validOptions = {"partition_size",
//                                                        "first_login_script",
//                                                        "post_build_script",
//                                                        "post_build_script_no_chroot",
//                                                        "user_files"};
//         isValid_ &= checkUnknownOptions(rawImageInfo, validOptions);
//     }
// }

// bool ImageInfo::merge(const ImageInfo& target) {
//     auto isValid = true;
//     if (target.isPresent_) {
//         // TODO(kd): Finish
//         this->partitionSize = std::max(this->partitionSize, target.partitionSize);

//         this->userFiles.insert(
//             this->userFiles.end(), target.userFiles.begin(), target.userFiles.end());
//     }

//     return isValid;
// }

// BuildProcessConfig::BuildProcessConfig(const RawTOMLConfig& rawConfig) {
//     auto rawBuildConfig = getBaseTable_(rawConfig, "build_process", kDarkGreenColorCode);

//     if (isPresent_) {
//         isValid_ &= getTOMLVal(rawBuildConfig, "enable_custom_cache", enableCustomCache, true);
//         isValid_ &= getTOMLVal(rawBuildConfig, "custom_mock_script", mockScript, true);

//         const std::vector<std::string> validOptions = {"enable_custom_cache",
//         "custom_mock_script"}; isValid_ &= checkUnknownOptions(rawBuildConfig, validOptions);
//     }
// }

// void Repo::from_toml(const toml::value& v) {
//     auto table = v.as_table();

//     isValid_ &= getTOMLVal(table,
//                            "name",
//                            name,
//                            false,
//                            "repo's name needs to have type string, moving to next repo",
//                            "repo's name(string) is undefined, moving to next repo");
//     if (isValid_) {
//         spdlog::info("parsing repo: " + name);
//         isValid_ &= getTOMLVal(table, "display_name", displayName, false);

//         isValid_ &= getTOMLVal(table, "metalink", metaLink, true);
//         isValid_ &= getTOMLVal(table, "baseurl", baseurl, true);

//         if (metaLink.empty() && baseurl.empty()) {
//             spdlog::error("repo has neither metalink or baseurl");
//             isValid_ = false;
//         }

//         auto gpgcheckValid = getTOMLVal(table, "gpgcheck", gpgcheck, false);
//         isValid_ &= gpgcheckValid;
//         if (gpgcheckValid && gpgcheck) {
//             isValid_ &= getTOMLVal(table,
//                                    "gpgkey",
//                                    gpgkey,
//                                    false,
//                                    "gpgkey needs to have type string when gpgcheck is true",
//                                    "gpgkey(string) needs to be defined when gpgcheck is true");
//         }

//         const std::vector<std::string> validOptions = {
//             "name", "display_name", "metalink", "baseurl", "gpgcheck", "gpgkey"};
//         isValid_ &= checkUnknownOptions(table, validOptions);
//     }
// }

// RepoConfig::RepoConfig(const RawTOMLConfig& rawConfig) {
//     auto rawRepoConfig = getBaseTable_(rawConfig, "repo", kGreenColorCode);

//     if (isPresent_) {
//         isValid_ &= getTOMLVal(rawRepoConfig, "standard_repos", standardRepos, true);
//         isValid_ &= getTOMLVal(rawRepoConfig, "copr_repos", coprRepos, true);

//         getTOMLVal(rawRepoConfig, "custom_repos", customRepos, true);
//         isValid_ &= std::accumulate(
//             customRepos.begin(), customRepos.end(), true, [](bool isValid, Repo const& repo) {
//                 return isValid && repo;
//             });

//         const std::vector<std::string> validOptions = {
//             "standard_repos", "copr_repos", "custom_repos"};
//         isValid_ &= checkUnknownOptions(rawRepoConfig, validOptions);
//     }
// }

// PackageSet::PackageSet() {}

// PackageSet::PackageSet(const toml::table& rawPackageConfig, const std::string& tableName) {
//     auto setConfig = getBaseTable_(rawPackageConfig, tableName);

//     if (isPresent_) {
//         spdlog::info("parsing package set: " + tableName);
//         isValid_ &= getTOMLVal(setConfig, "install", installList, true);
//         isValid_ &= getTOMLVal(setConfig, "remove", removeList, true);

//         const std::vector<std::string> validOptions = {"install", "remove"};
//         isValid_ &= checkUnknownOptions(setConfig, validOptions);
//     }
// }

// PackageConfig::PackageConfig(const RawTOMLConfig& rawConfig) {
//     auto rawPackageConfig = getBaseTable_(rawConfig, "package", kYellowColorCode);

//     if (isPresent_) {
//         rpm      = PackageSet(rawPackageConfig, "rpm");
//         rpmGroup = PackageSet(rawPackageConfig, "rpm_group");
//         isValid_ &= rpm && rpmGroup;

//         const std::vector<std::string> validOptions = {"rpm", "rpm_group"};
//         isValid_ &= checkUnknownOptions(rawPackageConfig, validOptions);
//     }
// }

// MiscConfig::MiscConfig(const RawTOMLConfig& rawConfig) {
//     auto rawMiscConfig = getBaseTable_(rawConfig, "misc", kBlueColorCode);

//     if (isPresent_) {
//         isValid_ &= getTOMLVal(rawMiscConfig, "language", language, true);
//         isValid_ &= getTOMLVal(rawMiscConfig, "keyboard", keyboard);
//         isValid_ &= getTOMLVal(rawMiscConfig, "timezone", timezone);

//         const std::vector<std::string> validOptions = {"language", "keyboard", "timezone"};
//         isValid_ &= checkUnknownOptions(rawMiscConfig, validOptions);
//     }
// }

}  // namespace hatter