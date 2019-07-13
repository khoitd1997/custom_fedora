#include "logger.hpp"

#define SPDLOG_LEVEL_NAMES \
    { "[trace]", "[debug]", "[info]", "[warning]", "[error]", "[critical]", "[]" }

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <regex>

#include "build_variable.hpp"

namespace hatter {
namespace logger {
static const auto logPattern = "%^%-9l%$ %v";
void              init() {
    std::vector<spdlog::sink_ptr> sinks;

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::warn);
    consoleSink->set_pattern(logPattern);
    sinks.push_back(consoleSink);

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        build_variable::kParserLogPath.string(), true);
    fileSink->set_level(spdlog::level::trace);
    fileSink->set_pattern(logPattern);
    sinks.push_back(fileSink);

    auto logger = std::make_shared<spdlog::logger>(
        "hatter_parser_logger", std::begin(sinks), std::end(sinks));
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);

    spdlog::info("hatter log initialized");
}
void info(const std::string& message) { spdlog::info(message); }
void warn(const std::string& message) { spdlog::warn(message); }

void error(const std::string& message) { spdlog::error(message); }
void skipLine(const int totalLine) {
    spdlog::set_pattern("");
    for (auto i = 0; i < totalLine; ++i) { spdlog::info(""); }
    spdlog::set_pattern(logPattern);
}
}  // namespace logger
}  // namespace hatter