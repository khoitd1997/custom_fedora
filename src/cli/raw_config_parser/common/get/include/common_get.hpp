#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"
#include "toml_utils.hpp"

namespace hatter {
std::shared_ptr<TOMLError> getFilePath(toml::table&                        rawConf,
                                       const std::string&                  keyName,
                                       const std::filesystem::path&        fileDir,
                                       std::vector<std::filesystem::path>& out,
                                       const bool                          singleFile = true);
}  // namespace hatter