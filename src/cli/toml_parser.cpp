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
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "toml11/toml.hpp"

#include "config_builder.hpp"
#include "raw_config_parser.hpp"
#include "utils.hpp"

// #include "cpptoml.hpp"

static void logInit() {
    auto consoleLog = spdlog::stdout_color_mt("console");

    spdlog::set_default_logger(consoleLog);
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%^%l%$] %v");

    spdlog::info("hatter log initialized");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    logInit();

    // TODO(kd): error handling here
    auto data = toml::parse(argv[1]);

    hatter::BasicConfig basicConfig(data);
    if (!basicConfig) { std::cout << "Failed to parse basic config" << std::endl; }
    std::cout << "image arch: " << basicConfig.imageArch << std::endl;
    // auto                buildDir = hatter::getExeDir() + "/build";
    // hatter::buildMockConfig(basicConfig, buildDir);

    hatter::RepoConfig repoConfig(data);
    // if (!repoConfig) { std::cout << "Failed to parse repo config" << std::endl; }
    // if (!repoConfig.customRepos.empty()) {
    //     std::cout << repoConfig.customRepos.at(0).name << std::endl;
    //     std::cout << repoConfig.customRepos.at(0).metaLink << std::endl;
    //     std::cout << repoConfig.customRepos.at(0).baseurl << std::endl;
    //     std::cout << repoConfig.customRepos.at(0).gpgcheck << std::endl;
    //     std::cout << repoConfig.customRepos.at(0).gpgkey << std::endl;
    // }

    hatter::PackageConfig packageConfig(data);
    if (!packageConfig) {
        std::cout << "Failed to parse repo config" << std::endl;
    } else {
        std::cout << "rpm: " << std::endl;
        if (packageConfig.rpm.installList.size() > 0) {
            std::cout << "install: " << packageConfig.rpm.installList.at(0) << std::endl;
        }
        if (packageConfig.rpm.removeList.size() > 0) {
            std::cout << "remove: " << packageConfig.rpm.removeList.at(0) << std::endl;
        }

        std::cout << "rpm_group: " << std::endl;
        if (packageConfig.rpmGroup.installList.size() > 0) {
            std::cout << "install: " << packageConfig.rpmGroup.installList.at(0) << std::endl;
        }
        if (packageConfig.rpmGroup.removeList.size() > 0) {
            std::cout << "remove: " << packageConfig.rpmGroup.removeList.at(0) << std::endl;
        }
    }

    // const auto rawBasicConfig = toml::get<toml::Table>(data.at("basic"));
    // const auto title = toml::get<std::string>(rawBasicConfig.at("mock_env_fedora_version"));
    // std::cout << rawBasicConfig << std::endl;
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

    //     std::shared_ptr<cpptoml::table> rawBasicConfig;
    //     hatter::BasicConfig             basicConfig;

    //     isValid_Config &= hatter::getTOMLVal(rawConfig.get(), "basic", rawBasicConfig);
    //     if (rawBasicConfig) {
    //         isValid_Config &= hatter::getBasicConfig(rawBasicConfig.get(), basicConfig);
    //     }

    //     if (!isValid_Config) {
    //         spdlog::error("Invalid configuration, exitting");
    //         return 1;
    //     }

    // } catch (const cpptoml::parse_exception& e) {
    //     std::cerr << "Failed to parse setting file: " << argv[1] << ": " << e.what() <<
    //     std::endl; return 1;
    // }

    return 0;
}
