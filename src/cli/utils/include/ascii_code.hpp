#pragma once

namespace hatter {
namespace ascii_code {
// formatting code
static const std::string kBold      = "\033[1m";
static const std::string kUnderline = "\033[4m";
static const std::string kItalic    = "\033[3m";
static const std::string kFaint     = "\033[2m";
static const std::string kReset     = "\033[0m";

// color code
static const std::string kBlue      = "\033[38;5;33m";
static const std::string kLightBlue = "\033[38;5;33m";

static const std::string kMidGreen      = "\033[38;5;084m";
static const std::string kLightMidGreen = "\033[38;5;156m";

static const std::string kLightGreen = "\033[38;5;106m";
static const std::string kGreen      = "\033[38;5;154m";
static const std::string kDarkGreen  = "\033[38;5;34m";

static const std::string kCyan      = "\033[38;5;051m";
static const std::string kLightCyan = "\033[38;5;123m";

static const std::string kYellow     = "\033[38;5;226m";
static const std::string kDarkYellow = "\033[38;5;220m";

static const std::string kGrey = "\033[38;5;246m";

// color code attached to meaning
static const auto kErrorListColor     = kDarkYellow;
static const auto kImportantWordColor = "\033[38;5;036m";
}  // namespace ascii_code
}  // namespace hatter