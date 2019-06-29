/**
 * @file toml_parser.cpp
 * @author Khoi Trinh
 * @brief
 * @version 0.1
 * @date 2019-04-07
 *
 * @copyright Copyright Khoi Trinh (c) 2019
 *
 */

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

#include "toml11/toml.hpp"

#include "build_variable.hpp"
#include "config_builder.hpp"
#include "logger.hpp"
#include "raw_config_parser.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    hatter::logger::init();
    std::cout << std::endl;

    auto               filePath = std::filesystem::path(std::string(argv[1]));
    hatter::FullConfig currConfig;
    auto               hasError = hatter::getFullConfig(filePath, currConfig);

    const hatter::build_variable::CLIBuildVariable currBuildVar;
    hatter::build_variable::CLIBuildVariable       prevBuildVar;

    if (!currBuildVar.parserMode) {
        if (!hasError) {
            hatter::FullConfig prevConfig;
            if (!hatter::build_variable::kIsFirstBuild) {
                prevBuildVar = hatter::build_variable::CLIBuildVariable{"prev"};
                hatter::getFullConfig(hatter::build_variable::kPrevParentConfigPath, prevConfig);
            }
            hatter::config_builder::build(currConfig, prevConfig, currBuildVar, prevBuildVar);
        } else {
            hatter::logger::error("failed to parse");
            return 1;
        }
    }

    return 0;
    // hatter::TOMLConfigFile conf(fileName);

    // if (!conf) { std::cout << "Failed to get conf file" << std::endl; }

    // auto        isValid  = parseInputTOMLFile(argv[1], data);
    // auto        notEmpty = !data.empty();

    // if (isValid && notEmpty) {
    //     hatter::DistroInfo distroInfo(data);
    //     if (!distroInfo) { std::cout << "Failed to parse basic config" << std::endl; }
    //     // std::cout << "image arch: " << distroInfo.imageArch << std::endl;
    //     // auto                buildDir = hatter::getExeDir() + "/build";
    //     // hatter::buildMockConfig(distroInfo, buildDir);
    //     hatter::ImageInfo imageInfo(data);

    //     hatter::BuildProcessConfig buildProcessConfig(data);

    //     hatter::RepoConfig repoConfig(data);
    //     // if (!repoConfig) { std::cout << "Failed to parse repo config" << std::endl; }
    //     // if (!repoConfig.customRepos.empty()) {
    //     //     std::cout << repoConfig.customRepos.at(0).name << std::endl;
    //     //     std::cout << repoConfig.customRepos.at(0).metaLink << std::endl;
    //     //     std::cout << repoConfig.customRepos.at(0).baseurl << std::endl;
    //     //     std::cout << repoConfig.customRepos.at(0).gpgcheck << std::endl;
    //     //     std::cout << repoConfig.customRepos.at(0).gpgkey << std::endl;
    //     // }
    //     hatter::MiscConfig miscConfig(data);

    //     hatter::PackageConfig packageConfig(data);
    //     if (!packageConfig) {
    //         std::cout << "Failed to parse repo config" << std::endl;
    //     } else {
    //         std::cout << "rpm: " << std::endl;
    //         if (packageConfig.rpm.installList.size() > 0) {
    //             std::cout << "install: " << packageConfig.rpm.installList.at(0) << std::endl;
    //         }
    //         if (packageConfig.rpm.removeList.size() > 0) {
    //             std::cout << "remove: " << packageConfig.rpm.removeList.at(0) << std::endl;
    //         }

    //         std::cout << "rpm_group: " << std::endl;
    //         if (packageConfig.rpmGroup.installList.size() > 0) {
    //             std::cout << "install: " << packageConfig.rpmGroup.installList.at(0) <<
    //             std::endl;
    //         }
    //         if (packageConfig.rpmGroup.removeList.size() > 0) {
    //             std::cout << "remove: " << packageConfig.rpmGroup.removeList.at(0) << std::endl;
    //         }
    //     }
    // }

    // const auto rawDistroInfo = toml::get<toml::Table>(data.at("basic"));
    // const auto title = toml::get<std::string>(rawDistroInfo.at("mock_env_fedora_version"));
    // std::cout << rawDistroInfo << std::endl;
    // std::cout << "the mock env version is " << title << std::endl;

    // auto& testTable = toml::get<toml::Table>(data.at("basic"));
    // toml::table testTable;
    // auto        res = hatter::getTOMLVal(data, "basic", testTable);

    // std::string testVal;
    // res &= hatter::getTOMLVal<std::string>(testTable, "base_spin", testVal, "BASIC");
    // if (!res) {
    //     std::cout << "Failed to get table" << std::endl;
    //     exit(1);
    // }
    // std::cout << testVal << std::endl;

    // const auto testStr   = toml::get<toml::string>(testTable.at("mock_env_fedora_version"));
    // if (testTable.is_ok()) {
    //     std::cout << "test table is valid" << std::endl;
    // } else {
    //     std::cout << "test table is invalid" << std::endl;
    // }
    // testStr = "20";
    // testTable["mock_env_fedora_version"] = "20";
    // std::cout << data.at("basic") << std::endl;

    // nums = [1, 2, 3, 4, 5]
    // std::vector<int> nums = toml::get<std::vector<int>>(data.at("nums"));
    // std::cout << "the length of `nums` is" << nums.size() << std::endl;

    // std::cout << "Current path is " << getExeDir() << std::endl;

    // try {
    //     auto rawConfig     = cpptoml::parse_file(argv[1]);
    //     auto isValid_Config = true;

    //     std::shared_ptr<cpptoml::table> rawDistroInfo;
    //     hatter::DistroInfo             distroInfo;

    //     isValid_Config &= hatter::getTOMLVal(rawConfig.get(), "basic", rawDistroInfo);
    //     if (rawDistroInfo) {
    //         isValid_Config &= hatter::getDistroInfo(rawDistroInfo.get(), distroInfo);
    //     }

    //     if (!isValid_Config) {
    //         spdlog::error("Invalid configuration, exitting");
    //         return 1;
    //     }

    // } catch (const cpptoml::parse_exception& e) {
    //     std::cerr << "Failed to parse setting file: " << argv[1] << ": " << e.what() <<
    //     std::endl; return 1;
    // }
}
