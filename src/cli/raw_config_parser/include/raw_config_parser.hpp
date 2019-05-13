#pragma once

#include <filesystem>

#include "toml11/toml.hpp"

#include "type.hpp"

namespace hatter {

bool testGet(toml::table& t);

bool testGetFile(std::filesystem::path& filePath, FullConfig& fullConfig);
}  // namespace hatter