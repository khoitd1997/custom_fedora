#include "raw_config_parser.hpp"

// #include <spdlog/logger.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/spdlog.h>

// #include <algorithm>
// #include <functional>
// #include <iostream>
// #include <numeric>

// #include "toml11/toml.hpp"

// #include "toml_utils.hpp"
// #include "utils.hpp"

#include <iostream>

#include "toml_section_parser.hpp"
#include "toml_utils.hpp"

// static const auto kResetColorCode      = "\033[0m";
// static const auto kCyanColorCode       = "\033[38;5;087m";
// static const auto kBlueColorCode       = "\033[38;5;12m";
// static const auto kYellowColorCode     = "\033[38;5;226m";
// static const auto kDarkYellowColorCode = "\033[38;5;220m";
// static const auto kDarkGreenColorCode  = "\033[38;5;34m";
// static const auto kGreenColorCode      = "\033[38;5;154m";

namespace hatter {

void printAllError(const internal::TopSectionErrorReport& error) {
    if (error.hasError()) {
        std::cout << "Topsection name: " << error.sectionName << std::endl;
        for (const auto& err : error.subSectionErrors) {
            std::cout << "Subsection name: " << err.sectionName << std::endl;
            for (const auto& tomlErr : err.tomlErrors) {
                std::cout << tomlErr->what() << std::endl;
            }

            for (const auto& sanitizerError : err.sanitizerErrors) {
                std::cout << sanitizerError->what() << std::endl;
            }
        }

        std::cout << std::endl << "Top section error" << std::endl;
        for (const auto& topTOMLErr : error.tomlErrors) {
            std::cout << topTOMLErr->what() << std::endl;
        }
        for (const auto& sanitizerError : error.sanitizerErrors) {
            std::cout << sanitizerError->what() << std::endl;
        }
    }
}

bool testGet(toml::table& t) {
    RepoConfig repoConf;
    auto       error = internal::getSection(t, repoConf);

    printAllError(error);

    auto                     tempTable = t;
    std::vector<std::string> includeFiles;
    getTOMLVal(tempTable, "include_files", includeFiles);
    std::cout << "Include files:" << std::endl << std::endl;
    auto hasMergeError = false;
    for (const auto& file : includeFiles) {
        std::cout << "Parsing file:" << file << std::endl;
        auto childTable = toml::parse(file);

        RepoConfig childConf;
        auto       childError = internal::getSection(childTable, childConf);

        std::cout << "Standard Repos:" << std::endl;
        for (const auto& stdRepo : childConf.standardRepos) { std::cout << stdRepo << std::endl; }

        printAllError(childError);

        if (!hasMergeError && !childError.hasError()) {
            auto mergeError = internal::merge(repoConf, childConf);

            if (mergeError.hasError()) {
                std::cout << "Merge error:" << std::endl;
                for (const auto& mError : mergeError.errors) {
                    std::cout << mError.what() << std::endl;
                }
            }

            hasMergeError = mergeError.hasError();
        }

        std::cout << std::endl;
    }

    // std::cout << "printing final repo list:" << std::endl;
    // for (const auto& repo : repoConf.standardRepos) { std::cout << repo << std::endl; }

    std::cout << "printing final custom repo list:" << std::endl;
    for (const auto& repo : repoConf.customRepos) { std::cout << repo.name << std::endl; }

    // return error.hasError;
    return false;
}
// namespace {
// // TODO(kd): Refactor this into a log formatting module if needed
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
//     auto rawImageInfo = getBaseTable_(rawConfig, "image_info", kDarkYellowColorCode);

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

// RawTOMLConfig::RawTOMLConfig(const std::string& filePath) {
//     try {
//         config   = toml::parse(filePath);
//         isValid_ = true;
//     } catch (const std::runtime_error& e) {
//         spdlog::error("failed to parse file " + filePath + " with error: " + e.what());
//     } catch (const toml::syntax_error& e) {
//         spdlog::error("syntax error in file " + filePath + " with error:\n" + e.what());
//     }
// }

// TOMLConfigFile::TOMLConfigFile(const std::string& filePath)
//     : TOMLConfigFile(RawTOMLConfig(filePath)) {}

// TOMLConfigFile::TOMLConfigFile(const RawTOMLConfig& rawConfig)
//     : distroInfo{rawConfig}, imageInfo{rawConfig} {
//     if (rawConfig) {
//         std::vector<std::string> includeFiles;
//         isValid_ &= getTOMLVal(rawConfig.config, "include_files", includeFiles, true);

//         if (isValid_) {
//             for (auto i = includeFiles.crbegin(); i != includeFiles.crend(); ++i) {
//                 std::cout << "processing include file: " << *i << std::endl;
//                 TOMLConfigFile childConf(*i);
//                 if (!childConf) {
//                     isValid_ = false;
//                     std::cout << "child config invalid: " << *i << std::endl;
//                 } else {
//                     isValid_ &= this->merge(childConf);
//                 }
//             }
//         }
//     } else {
//         isValid_ = false;
//     }
// }

// bool TOMLConfigFile::merge(const TOMLConfigFile& configFile) {
//     auto isValid = true;

//     isValid &= this->distroInfo.merge(configFile.distroInfo);

//     return isValid;
// }

}  // namespace hatter