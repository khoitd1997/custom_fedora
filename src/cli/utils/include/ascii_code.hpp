#pragma once

namespace hatter {
namespace ascii_code {
// formatting code
static const auto kBold      = "\033[1m";
static const auto kUnderline = "\033[4m";
static const auto kItalic    = "\033[3m";
static const auto kFaint     = "\033[2m";
static const auto kReset     = "\033[0m";

// color code
static const auto kBlue       = "\033[38;5;12m";
static const auto kCyan       = "\033[38;5;087m";
static const auto kYellow     = "\033[38;5;226m";
static const auto kDarkYellow = "\033[38;5;220m";
static const auto kGreen      = "\033[38;5;154m";
static const auto kDarkGreen  = "\033[38;5;34m";
static const auto kGrey       = "\033[38;5;246m";
}  // namespace ascii_code
}  // namespace hatter