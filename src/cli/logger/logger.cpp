#include "logger.hpp"

#define SPDLOG_LEVEL_NAMES \
    { "[trace]", "[debug]", "[info]", "[warning]", "[error]", "[critical]", "[]" }

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <regex>

#include "build_variable.hpp"
#include "formatter.hpp"
#include "utils.hpp"

#include "ascii_format.hpp"

namespace hatter {
namespace logger {
namespace {
const std::string spacing = "\t  ";
std::string       formatMessage(const std::string& message) { return spacing + message + "\n"; }
}  // namespace

void init() { info("hatter log initialized"); }
void info(const std::string& message, const bool isStartAnimation) {
    std::cout << formatter::formatText("[info]", formatter::kDefaultGreen) + formatMessage(message)
              << std::flush;
    appendFile("[info]" + formatMessage(message), build_variable::kParserLogPath);
    if (isStartAnimation) {
        // move cursor up the line and start appending to previous line
        std::cout << "\033[A"
                  << "\33[" + std::to_string(message.length() + 11) + "C" << std::flush;
    }
}
void warn(const std::string& message) {
    std::cout << formatter::formatText("[warn]", formatter::kDarkYellow) + formatMessage(message)
              << std::flush;
    appendFile("[warn]" + formatMessage(message), build_variable::kParserLogPath);
}

void error(const std::string& message) {
    std::cout << formatter::formatText("[error]", formatter::kDefaultRed) + formatMessage(message)
              << std::flush;
    appendFile("[error]" + formatMessage(message), build_variable::kParserLogPath);
}
void skipLine(const int totalLine) {
    const std::string lineStr(static_cast<size_t>(totalLine), '\n');
    std::cout << lineStr << std::flush;
    appendFile(lineStr, build_variable::kParserLogPath);
}
}  // namespace logger
}  // namespace hatter