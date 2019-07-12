#pragma once

#include <filesystem>

#include "toml11/toml.hpp"

#include "build_variable.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
bool getFullConfig(const std::filesystem::path&            filePath,
                   const build_variable::CLIBuildVariable& currBuildVar,
                   FullConfig&                             fullConfig,
                   const bool                              isGetPrev = false);
}  // namespace hatter