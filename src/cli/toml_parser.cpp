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
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <chrono>
#include <ctime>
#include <iomanip>

#include "mock_config.hpp"

#include "cpptoml.hpp"

#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

static std::string getExeDir(void) {
    char buff[PATH_MAX];
    auto len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len != -1) {
        buff[len] = '\0';
        return std::string(dirname(buff));
    }

    return std::string("");
}

static void logInit(const std::string& osName, const std::string& buildDir) {
    typedef std::chrono::system_clock sysclock;
    auto                              now = sysclock::to_time_t(sysclock::now());
    auto                              ss  = std::stringstream();
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d-%T");
    auto logName = osName + "-" + ss.str();
    auto logPath = buildDir + "/logs/" + logName + ".log";

    auto consoleLog = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleLog->set_level(spdlog::level::warn);
    auto fileLog = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
    fileLog->set_level(spdlog::level::debug);
    std::vector<spdlog::sink_ptr> logs{consoleLog, fileLog};

    auto logger = std::make_shared<spdlog::logger>("parser_log", logs.begin(), logs.end());
    spdlog::set_default_logger(logger);
    spdlog::info("hatter log initialized");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::cout << "Current path is " << getExeDir() << std::endl;
    auto buildDir = getExeDir() + "/build";

    try {
        auto config = cpptoml::parse_file(argv[1]);

        // TODO(kd): add more error checking
        auto buildConfig = config->get_table("build");
        mockconfig::verify(buildConfig.get());

        // TODO(kd): verify build info before log init
        logInit(*(buildConfig->get_as<std::string>("os_name")), buildDir);

        // mockconfig::build(buildConfig.get(), buildDir);
    } catch (const cpptoml::parse_exception& e) {
        std::cerr << "Failed to parse " << argv[1] << ": " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
