#include "raw_config_parser.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>

#include "logger.hpp"

#include "build_variable.hpp"
#include "error_report_file_type.hpp"
#include "toml_utils.hpp"

#include "build_process_handler.hpp"
#include "distro_info_handler.hpp"
#include "image_info_handler.hpp"
#include "misc_handler.hpp"
#include "package_handler.hpp"
#include "repo_handler.hpp"

namespace hatter {
namespace {
typedef std::function<std::vector<TopSectionErrorReport>(
    toml::table&, const std::filesystem::path& filePath, FullConfig&)>
    ParseFunc;

typedef std::function<std::vector<TopSectionErrorReport>(FullConfig&, const FullConfig&)> MergeFunc;

bool depthFirstSearch(const std::filesystem::path& filePath,
                      const std::string&           parentFileName,
                      FullConfig&                  fullConfig,
                      FullErrorReport&             fullReport,
                      ParseFunc                    parseFunc,
                      MergeFunc                    mergeFunc) {
    auto currDirectory = filePath.parent_path().string();
    auto currFileName  = filePath.filename().string();

    // TODO(kd): error handling here
    auto rawConfig = toml::get<toml::table>(toml::parse(filePath));

    // TODO(kd): error handling here
    std::vector<std::string> includeFiles;
    getNonMemberTOMLVal(rawConfig, "include_files", includeFiles);
    // std::cout << "current file: " << filePath << std::endl;

    FileSectionErrorReport fileSectionReport(currFileName, parentFileName);

    fileSectionReport.add(parseFunc(rawConfig, filePath, fullConfig));

    fullReport.add(std::make_shared<FileSectionErrorReport>(fileSectionReport));

    // std::cout << "First Login Path:" << std::endl;
    // for (const auto& filePath : fullConfig.imageInfo.firstLoginScripts.value) {
    //     std::cout << filePath.string() << std::endl;
    // }

    for (const auto& childFile : includeFiles) {
        auto childPath = std::filesystem::path(
            (currDirectory == "") ? childFile : currDirectory + "/" + childFile);
        auto childFileName = childPath.filename().string();
        auto childTable    = toml::parse(childPath.string());

        FullConfig childConf;
        depthFirstSearch(childPath, currFileName, childConf, fullReport, parseFunc, mergeFunc);

        if (!fullReport) {
            FileMergeErrorReport mergeReport(currFileName, childFileName);

            mergeReport.add(mergeFunc(fullConfig, childConf));

            fullReport.add(std::make_shared<FileMergeErrorReport>(mergeReport));
        } else {
        }
    }

    return fullReport.operator bool();
}

bool getFile(const std::filesystem::path& filePath,
             const std::string&           parentFileName,
             FullConfig&                  fullConfig,
             FullErrorReport&             fullReport,
             const bool                   getRepoFailed) {
    auto parseFunc = ParseFunc{[getRepoFailed](toml::table&                 rawConfig,
                                               const std::filesystem::path& filePath,
                                               FullConfig&                  fullConfig) {
        auto ret = std::vector<TopSectionErrorReport>{
            distro_info_handler::parse(rawConfig, fullConfig.distroInfo),
            image_info_handler::parse(rawConfig, filePath.parent_path(), fullConfig.imageInfo),
            build_process_handler::parse(rawConfig, filePath.parent_path(), fullConfig.buildConfig),
            misc_handler::parse(rawConfig, fullConfig.miscConfig)};

        if (!getRepoFailed) {
            ret.push_back(package_handler::parse(rawConfig, fullConfig.packageConfig));
        }

        return ret;
    }};
    auto mergeFunc = MergeFunc{[](FullConfig& parentConfig, const FullConfig& childConfig) {
        return std::vector<TopSectionErrorReport>{
            distro_info_handler::merge(parentConfig.distroInfo, childConfig.distroInfo),
            image_info_handler::merge(parentConfig.imageInfo, childConfig.imageInfo),
            build_process_handler::merge(parentConfig.buildConfig, childConfig.buildConfig),
            package_handler::merge(parentConfig.packageConfig, childConfig.packageConfig),
            misc_handler::merge(parentConfig.miscConfig, childConfig.miscConfig)};
    }};
    return depthFirstSearch(filePath, parentFileName, fullConfig, fullReport, parseFunc, mergeFunc);
}
bool getRepo(const std::filesystem::path& filePath,
             const std::string&           parentFileName,
             FullConfig&                  fullConfig,
             FullErrorReport&             fullReport) {
    auto parseFunc = ParseFunc{
        [](toml::table& rawConfig, const std::filesystem::path& filePath, FullConfig& fullConfig) {
            (void)(filePath);
            return std::vector<TopSectionErrorReport>{
                repo_handler::parse(rawConfig, fullConfig.repoConfig)};
        }};
    auto       mergeFunc = MergeFunc{[](FullConfig& parentConfig, const FullConfig& childConfig) {
        return std::vector<TopSectionErrorReport>{
            repo_handler::merge(parentConfig.repoConfig, childConfig.repoConfig)};
    }};
    const auto failed =
        depthFirstSearch(filePath, parentFileName, fullConfig, fullReport, parseFunc, mergeFunc);
    // TODO(kd): Add repo command here
    return failed;
}

}  // namespace

bool getFullConfig(const std::filesystem::path& filePath,
                   FullConfig&                  fullConfig,
                   const bool                   suppressOuput) {
    FullErrorReport fullReport;

    const auto getRepoFailed = getRepo(filePath, "", fullConfig, fullReport);
    getFile(filePath, "", fullConfig, fullReport, getRepoFailed);
    if (fullReport) {
        if (!suppressOuput) {
            // std::cout << "Printing out error" << std::endl;
            fullReport.what();
        }
        return true;
    }
    return false;
}
}  // namespace hatter