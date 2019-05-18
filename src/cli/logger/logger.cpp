#include "logger.hpp"

#define SPDLOG_LEVEL_NAMES \
    { "[trace]", "[debug]", "[info]", "[warning]", "[error]", "[critical]", "[]" }

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace hatter {
namespace logger {
void init() {
    auto consoleLog = spdlog::stdout_color_mt("console");

    spdlog::set_default_logger(consoleLog);
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("%^%-9l%$ %v");

    spdlog::info("hatter log initialized");
}
void info(const std::string& message) { spdlog::info(message); }
void warn(const std::string& message) { spdlog::warn(message); }
void error(const std::string& message) { spdlog::error(message); }
}  // namespace logger
}  // namespace hatter