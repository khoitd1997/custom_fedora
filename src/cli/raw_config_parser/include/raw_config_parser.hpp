#pragma once

#include <filesystem>

#include "toml11/toml.hpp"

#include "hatter_config_type.hpp"

namespace hatter {
bool getFullConfig(const std::filesystem::path& filePath, FullConfig& fullConfig);
}  // namespace hatter