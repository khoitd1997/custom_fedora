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

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "config_builder.hpp"
#include "raw_config_parser.hpp"
#include "utils.hpp"

#include "cpptoml.hpp"

static std::string getExeDir(void) {
    char buff[PATH_MAX];
    auto len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len != -1) {
        buff[len] = '\0';
        return std::string(dirname(buff));
    }

    return std::string("");
}

static void logInit(const std::string& buildDir) {
    auto logPath = buildDir + "/logs/" + "pre_build.log";

    auto consoleLog = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleLog->set_level(spdlog::level::warn);
    auto fileLog = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
    fileLog->set_level(spdlog::level::debug);
    std::vector<spdlog::sink_ptr> logs{consoleLog, fileLog};
    // TODO(kd): Change log format

    auto logger = std::make_shared<spdlog::logger>("pre_build_log", logs.begin(), logs.end());
    spdlog::set_default_logger(logger);
    spdlog::info("hatter log initialized");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    // std::cout << "Current path is " << getExeDir() << std::endl;
    auto buildDir = getExeDir() + "/build";

    try {
        logInit(buildDir);
        auto rawConfig     = cpptoml::parse_file(argv[1]);
        auto isValidConfig = true;

        std::shared_ptr<cpptoml::table> rawBasicConfig;
        hatter::BasicConfig             basicConfig;

        isValidConfig &= hatter::getTOMLTable(rawConfig.get(), "basic", rawBasicConfig);
        if (rawBasicConfig) {
            isValidConfig &= hatter::getBasicConfig(rawBasicConfig.get(), basicConfig);
        }

        if (!isValidConfig) {
            spdlog::error("Invalid configuration, exitting");
            return 1;
        }

        hatter::buildMockConfig(basicConfig, buildDir);
    } catch (const cpptoml::parse_exception& e) {
        std::cerr << "Failed to parse setting file: " << argv[1] << ": " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
