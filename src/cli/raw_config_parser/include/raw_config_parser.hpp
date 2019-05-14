#pragma once

#include <filesystem>

#include "toml11/toml.hpp"

#include "hatter_config_type.hpp"

namespace hatter {
bool testGetFile(std::filesystem::path& filePath, FullConfig& fullConfig);
}  // namespace hatter