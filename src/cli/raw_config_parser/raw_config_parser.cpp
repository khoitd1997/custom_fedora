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
bool depthFirstSearch(
    const std::filesystem::path&                                           filePath,
    const std::string&                                                     parentFileName,
    FullConfig&                                                            fullConfig,
    FullErrorReport&                                                       fullReport,
    std::function<std::vector<TopSectionErrorReport>(
        toml::table&, const std::filesystem::path& filePath, FullConfig&)> parseFunc,
    std::function<std::vector<TopSectionErrorReport>(FullConfig&, const FullConfig&)> mergeFunc) {
    auto currDirectory = filePath.parent_path().string();
    auto currFileName  = filePath.filename().string();
    auto failed        = false;

    // TODO(kd): error handling here
    auto rawConfig = toml::get<toml::table>(toml::parse(filePath));

    // TODO(kd): error handling here
    std::vector<std::string> includeFiles;
    getNonMemberTOMLVal(rawConfig, "include_files", includeFiles);
    // std::cout << "current file: " << filePath << std::endl;

    FileSectionErrorReport fileSectionReport(currFileName, parentFileName);

    fileSectionReport.add(parseFunc(rawConfig, filePath, fullConfig));

    fullReport.add(std::make_shared<FileSectionErrorReport>(fileSectionReport));
    failed = failed || fileSectionReport;

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
        failed |=
            depthFirstSearch(childPath, currFileName, childConf, fullReport, parseFunc, mergeFunc);

        if (!failed) {
            FileMergeErrorReport mergeReport(currFileName, childFileName);

            mergeReport.add(mergeFunc(fullConfig, childConf));

            fullReport.add(std::make_shared<FileMergeErrorReport>(mergeReport));
            failed = failed || mergeReport;
        } else {
        }
    }

    return failed;
}

bool getFile(const std::filesystem::path& filePath,
             const std::string&           parentFileName,
             FullConfig&                  fullConfig,
             FullErrorReport&             fullReport) {
    auto parseFunc = std::function<std::vector<TopSectionErrorReport>(
        toml::table&, const std::filesystem::path& filePath, FullConfig&)>{
        [](toml::table& rawConfig, const std::filesystem::path& filePath, FullConfig& fullConfig) {
            return std::vector<TopSectionErrorReport>{
                distro_info_handler::parse(rawConfig, fullConfig.distroInfo),
                repo_handler::parse(rawConfig, fullConfig.repoConfig),
                package_handler::parse(rawConfig, fullConfig.packageConfig),
                misc_handler::parse(rawConfig, fullConfig.miscConfig),
                build_process_handler::parse(
                    rawConfig, filePath.parent_path(), fullConfig.buildConfig),
                image_info_handler::parse(rawConfig, filePath.parent_path(), fullConfig.imageInfo)};
        }};
    auto mergeFunc =
        std::function<std::vector<TopSectionErrorReport>(FullConfig&, const FullConfig&)>{
            [](FullConfig& parentConfig, const FullConfig& childConfig) {
                return std::vector<TopSectionErrorReport>{
                    distro_info_handler::merge(parentConfig.distroInfo, childConfig.distroInfo),
                    image_info_handler::merge(parentConfig.imageInfo, childConfig.imageInfo),
                    repo_handler::merge(parentConfig.repoConfig, childConfig.repoConfig),
                    package_handler::merge(parentConfig.packageConfig, childConfig.packageConfig),
                    misc_handler::merge(parentConfig.miscConfig, childConfig.miscConfig)};
            }};
    return depthFirstSearch(filePath, parentFileName, fullConfig, fullReport, parseFunc, mergeFunc);
}
}  // namespace

bool getFullConfig(const std::filesystem::path& filePath,
                   FullConfig&                  fullConfig,
                   const bool                   suppressOuput) {
    FullErrorReport fullReport;
    getFile(filePath, "", fullConfig, fullReport);
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